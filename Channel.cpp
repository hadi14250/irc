#include "Channel.hpp"

Channel::Channel() :
						_name(""),
						_topic(""),
						_topicAuthor(""),
						_topiCreation(""),
						_pass(""),
						_curMemAmt(0),
						_maxMemAmt(0),
						_modes("lokit"),
						_inviteOnly(false),
						_userLimit(false)
{}

Channel::Channel(std::string name) :
						_name(name),
						_topic(""),
						_topicAuthor(""),
						_topiCreation(""),
						_pass(""),
						_curMemAmt(0),
						_maxMemAmt(0),
						_modes("lokit"),
						_inviteOnly(false),
						_userLimit(false)
{
	Server::_chanMap[name] = *this;
}

bool	Channel::chkIfMember(std::string user) {
	chnMemIt	it;
	for (it = _members.begin(); (it != _members.end()) && it->first->_nick.compare(user); it++);
	return (it == _members.end() ? false : true);
}

void	Channel::manipOper(std::string nick, bool promote) {
	chnMemIt	it = _members.begin();

	for (; (it != _members.end()) && it->first->_nick.compare(nick); it++);
	if (it == _members.end())
		return ;
	it->second = (promote ? true : false);
}

bool	Channel::chkIfOper(std::string nick) {
	chnMemIt	it;
	for (it = _members.begin(); (it != _members.end()) && it->first->_nick.compare(nick); it++);
	return (it == _members.end() ? false : it->second);
}

bool	Channel::joinChannel(Client& newMember, std::string password) {
	vecStrIt		it;

	if (chkIfMember(newMember._nick))
		newMember._messages.push_back(ERR_USERONCHANNEL(newMember._nick, newMember._nick, _name));
	else if (_userLimit && (_curMemAmt >= _maxMemAmt))
		newMember._messages.push_back(ERR_CHANNELISFULL(newMember._nick, _name));
	else if (_inviteOnly && ((it = find(newMember._invitations.begin(), newMember._invitations.end(), _name)) == newMember._invitations.end()))
		newMember._messages.push_back(ERR_INVITEONLYCHAN(newMember._nick, _name));
	else if (_pass.size() && _pass.compare(password))
		newMember._messages.push_back(ERR_BADCHANNELKEY(newMember._nick, _name));
	else {
		if (_inviteOnly)
			newMember._invitations.erase(it);
		newMember._channels.push_back(_name);
		_members[&newMember] = false;
		if (!_curMemAmt)
			_members[&newMember] = true;
		_curMemAmt++;
		return true;
	}
	return false;
}

std::vector<std::string>	Channel::getChannelMembers() {
	std::vector<std::string>	members;
	std::vector<std::string>	memberBatches;
	bool						endOfList = false;

	for (chnMemIt it = _members.begin(); it != _members.end(); it++)
		if (it->second)
			members.push_back("@" + it->first->_nick);
	for (chnMemIt it = _members.begin(); it != _members.end(); it++)
		if (!it->second)
			members.push_back(it->first->_nick);
	while (!endOfList) {
		std::string	batch;
		size_t		limit = 0;
		for (; (limit < members.size()) && (limit < 14); limit++) {
			if (!batch.empty())
				batch += " ";
			batch += members.at(limit);
		}
		if (limit < 14)
			endOfList = true;
		if (!batch.empty())
			memberBatches.push_back(batch);
	}
	return memberBatches;
}

void	Channel::msgChannel(Client& sender, std::string message) {
	chnMemIt	it = _members.begin();

	for (; it != _members.end(); it++)
		if ((it->first->_sockfd != sender._sockfd))
			Server::_pfdsMap[it->first->_sockfd]._messages.push_back(message + "\r\n");
}

void	Channel::handleModeO(Client& sender, char mode, bool addflag, std::string param) {
	std::map<std::string, int>::iterator	it = Server::_nickMap.find(param);

	if (it == Server::_nickMap.end())
		sender._messages.push_back(ERR_NOSUCHNICK(sender._nick, param));
	else if (!chkIfMember(param))
		sender._messages.push_back(ERR_USERNOTINCHANNEL(sender._nick, param, _name));
	else if (sender._sockfd == it->second) // if mate tries to oper him self again just drop it!
		return ;
	else {
		manipOper(param, addflag);
		addflag ? addFlag(mode) : removeflag(mode);
	}
}

void	Channel::handleModeK(Client& sender, char mode, bool addflag, std::string param) {
	if (addflag && _pass.size())
		sender._messages.push_back(ERR_KEYSET(sender._nick, _name));
	else if (addflag && (param.find(" ") != std::string::npos))
		sender._messages.push_back(ERR_INVALIDKEY(sender._nick, _name));
	else {
		_pass = (addflag ? param : "");
		addflag ? addFlag(mode) : removeflag(mode);
	}
}

bool	Channel::addFlag(char mode) {
	if (_activeMode.empty())
		_activeMode = "+";
	if (_activeMode.find(mode) == std::string::npos)
		_activeMode += mode;
	return true;
}

bool	Channel::removeflag(char mode) {
	std::string	activeModes;

	for (size_t i = 0; i < _activeMode.size(); i++) {
		if (_activeMode.at(i) == mode)
			continue ;
		activeModes += _activeMode.at(i);
	}
	_activeMode = (activeModes.size() == 1 ? "" : activeModes);
	return true;
}

void	Channel::handleModeL(Client& sender, char mode, bool addflag, std::string param) {
	if (!addflag) {
		_userLimit = false;
		_maxMemAmt = 0;
		removeflag(mode);
	} else {
		std::stringstream	ss;
		std::string			excess;

		if (param.at(0) == '-')
			sender._messages.push_back(ERR_INVALIDMODEPARAM(sender._nick, _name, 'l', param, "negative number? seriously!"));
		else if (!(ss >> _maxMemAmt))
			sender._messages.push_back(ERR_INVALIDMODEPARAM(sender._nick, _name, 'l', param, "not a number!"));
		else if ((ss >> excess) && excess.size())
			sender._messages.push_back(ERR_INVALIDMODEPARAM(sender._nick, _name, 'l', param, "not a number!"));
		else {
			_userLimit = true;
			addFlag(mode);
		}
	}
	(void)mode;
}

void	Channel::handleTypeBC(Client& sender, char mode, bool addflag, std::string param) {
	if ((mode == 'o' || addflag) && param.empty())
		sender._messages.push_back(ERR_NEEDMOREPARAMS(sender._nick, "MODE"));
	else if (mode == 'k')
		handleModeK(sender, mode, addflag, param);
	else if (mode == 'l')
		handleModeL(sender, mode, addflag, param);
	else if (mode == 'o')
		handleModeO(sender, mode, addflag, param);
}

void	Channel::handleTypeD(char mode, bool addflag) {
	if (addflag && addFlag(mode)) {
		mode == 'i' ? _inviteOnly = true : _topicOperOnly = true;
		
	} else if (removeflag(mode))
		mode == 't' ? _inviteOnly = !true : _topicOperOnly = !true;
}

std::string	Channel::getValues(void) {
	std::string	modeArgs = "lk";
	std::stringstream	ss;
	std::string	args;

	for (size_t i = 0; i < _activeMode.size(); i++) {
		if (modeArgs.find(_activeMode.at(i)) != std::string::npos) {
			if (_activeMode.at(i) == 'k')
				args = (args.empty() ? (" " + _pass) : _pass);
			if (_activeMode.at(i) == 'l') {
				ss << _maxMemAmt;
				args = (args.empty() ? (" " + ss.str()) : ss.str());
			}
		}
	}
	return args;
}

void	Channel::chanMode(Client& sender, std::string modes, std::string value) {
	std::string	printMode;
	std::string	printValue;
	bool		sign;

	if (!chkIfMember(sender._nick))
		sender._messages.push_back(ERR_NOTONCHANNEL(sender._nick, _name));
	else if (!chkIfOper(sender._nick))
		sender._messages.push_back(ERR_CHANOPRIVSNEEDED(sender._nick, _name));
	else if (modes.empty())
		sender._messages.push_back(RPL_CHANNELMODEIS(sender._nick, _name, _activeMode, getValues()));
	else {
		for (size_t i = 0; i < modes.size(); i++) {
			sign = (modes.at(i) == '-' ? 0 : (modes.at(i) == '+' ? 1 : sign));
			if (modes.at(i) == '-' || modes.at(i) == '+')
				i++;
			if (_modes.substr(0, 3).find_first_of(modes.at(i)) != std::string::npos)
				handleTypeBC(sender, modes.at(i), sign, getCmd(value));
			else if (_modes.substr(3, 2).find_first_of(modes.at(i)) != std::string::npos)
				handleTypeD(modes.at(i), sign);
			else
				std::cerr << "error";// unknown mode!
			printMode += (sign ? "+" : "-") + modes.substr(i, 1);
			std::cerr << "value = " << value << std::endl;
			std::cerr << "get cmd value = " << getCmd(value) << std::endl;
			if (_modes.substr(0, 3).find_first_of(modes.at(i)) != std::string::npos) {
				printValue += (printValue.empty() ? "" : ",") + (modes.substr(i, 1).compare("k") ? "-\\(\",)/-" : getCmd(value));
				value = removeCmd(value);
			}
			sender._messages.push_back(":" + sender._identifier + " MODE " + _name + " " + printMode + " " + printValue);
		}
	}
}

void	Channel::seTopic(std::string author, std::string topic) {
	std::time_t currentTime; std::time(&currentTime);

	_topiCreation = (std::string)std::ctime(&currentTime);
	_topiCreation = _topiCreation.substr(0, _topiCreation.size() - 1);
	_topicAuthor = author;
	_topic = topic;
}

std::vector<std::string>	Channel::geTopicAndTopiCreation(void) {
	std::vector<std::string>	vec;

	vec.push_back(_topic);
	vec.push_back(_topicAuthor);
	vec.push_back(_topiCreation);
	return vec;
}

//! tmp //////////////////////////////////////////////////////////////////////////////

void	Channel::printChan() {
	chnMemIt it = _members.begin();
	std::cerr << "print chann of ->" << _name << std::endl;
	for (; it != _members.end(); it++)
		std::cerr << it->first->_nick << "oper -> " << (it->second ? "yes" : "no") << std::endl;
}
