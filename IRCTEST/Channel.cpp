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

std::string	Channel::getChannelName() const {
	return _name;
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

bool	Channel::chkTopicFlag() {
	return _topicOperOnly;
}

bool	Channel::joinChannel(Client& newMember, std::string password) {
	vecStrIt		it;

	if (chkIfMember(newMember._nick))
		newMember._messages.push_back(ERR_USERONCHANNEL(newMember._nick, newMember._nick, _name));
	else if (_userLimit && ((_members.size() >= _maxMemAmt)))
		newMember._messages.push_back(ERR_CHANNELISFULL(newMember._nick, _name));
	else if (_inviteOnly && ((it = find(newMember._invitations.begin(), newMember._invitations.end(), _name)) == newMember._invitations.end()))
		newMember._messages.push_back(ERR_INVITEONLYCHAN(newMember._nick, _name));
	else if (_pass.size() && _pass.compare(password))
		newMember._messages.push_back(ERR_BADCHANNELKEY(newMember._nick, _name));
	else {
		if (_inviteOnly)
			newMember._invitations.erase(it);
		newMember._channels.push_back(this);
		_members[&newMember] = false;
		if (!_curMemAmt)
			_members[&newMember] = true;
		_curMemAmt++;
		for (chnMemIt it = _members.begin(); it != _members.end(); it++) {
			it->first->_messages.push_back(JOIN_MSG(newMember._identifier, _name));
		}
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

void	Channel::msgChannel(Client& sender, std::string msg) {
	chnMemIt	it = _members.begin();

	if (!chkIfMember(sender._nick)) {
		sender._messages.push_back(ERR_NOTONCHANNEL(sender._nick, _name));
		return ;
	}
	for (; it != _members.end(); it++)
		if ((it->first->_sockfd != sender._sockfd))
			Server::_pfdsMap[it->first->_sockfd]._messages.push_back(PRIV_MSG(it->first->_identifier, _name, ((msg.size() && msg.at(0) == ':') ? removeColon(msg) : getCmd(msg) )) );
}

bool	Channel::handleModeO(Client& sender, char mode, bool addflag, std::string param) {
	std::map<std::string, int>::iterator	it = Server::_nickMap.find(param);

	if (it == Server::_nickMap.end())
		sender._messages.push_back(ERR_NOSUCHNICK(sender._nick, param));
	else if (!chkIfMember(param))
		sender._messages.push_back(ERR_USERNOTINCHANNEL(sender._nick, param, _name));
	else if (sender._sockfd == it->second)
		return false;
	else {
		manipOper(param, addflag);
		addflag ? addFlag(mode) : removeflag(mode);
		return true;
	}
	return false;
}

bool	Channel::handleModeK(Client& sender, char mode, bool addflag, std::string param) {
	if (addflag && _pass.size())
		sender._messages.push_back(ERR_KEYSET(sender._nick, _name));
	else if (addflag && (param.find(" ") != std::string::npos))
		sender._messages.push_back(ERR_INVALIDKEY(sender._nick, _name));
	else {
		_pass = (addflag ? param : "");
		addflag ? addFlag(mode) : removeflag(mode);
		return true;
	}
	return false;
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

bool	Channel::handleModeL(Client& sender, char mode, bool addflag, std::string param) {
	if (!addflag) {
		_userLimit = false;
		_maxMemAmt = 0;
		removeflag(mode);
	} else {
		std::stringstream	ss;
		std::string			excess;

		ss << param;
		if (param.at(0) == '-')
			sender._messages.push_back(ERR_INVALIDMODEPARAM(sender._nick, _name, 'l', param, "negative number? seriously!"));
		else if (!(ss >> _maxMemAmt))
			sender._messages.push_back(ERR_INVALIDMODEPARAM(sender._nick, _name, 'l', param, "not a number1!"));
		else if ((ss >> excess) && excess.size())
			sender._messages.push_back(ERR_INVALIDMODEPARAM(sender._nick, _name, 'l', param, "not a number!"));
		else {
			_userLimit = true;
			addFlag(mode);
			return true;
		}
		return false;
	}
	return true;
}

bool	Channel::handleTypeBC(Client& sender, char mode, bool addflag, std::string param) {
	bool	returnValue = false;

	if ((mode == 'o' || addflag) && param.empty())
		sender._messages.push_back(ERR_NEEDMOREPARAMS(sender._nick, "MODE"));
	else if (mode == 'k')
		returnValue = handleModeK(sender, mode, addflag, param);
	else if (mode == 'l')
		returnValue = handleModeL(sender, mode, addflag, param);
	else if (mode == 'o')
		returnValue = handleModeO(sender, mode, addflag, param);
	return returnValue;
}

bool	Channel::handleTypeD(char mode, bool addflag) {
	if (addflag && addFlag(mode)) {
		mode == 'i' ? _inviteOnly = true : _topicOperOnly = true;
	} else if (removeflag(mode))
		mode == 'i' ? _inviteOnly = !true : _topicOperOnly = !true;
	return true;
}

std::string	Channel::getValues(void) {
	std::string	modeArgs = "lk";
	std::stringstream	ss;
	std::string	args;

	for (size_t i = 0; i < _activeMode.size(); i++) {
		if (modeArgs.find(_activeMode.at(i)) != std::string::npos) {
			if (_activeMode.at(i) == 'k')
				args += (args.size() ? (" " + _pass) : _pass);
			if (_activeMode.at(i) == 'l') {
				ss << _maxMemAmt;
				args += (args.size() ? (" " + ss.str()) : ss.str());
			}
		}
	}
	return args;
}

void	Channel::chanMode(Client& sender, std::string modes, std::string value) {
	std::string	printMode;
	std::string	printValue;
	bool		appendPrint;
	short		sign = -1;// if -1 default, 0 is - and 1 is +

	if (!chkIfMember(sender._nick))
		sender._messages.push_back(ERR_NOTONCHANNEL(sender._nick, _name));
	else if (!chkIfOper(sender._nick))
		sender._messages.push_back(ERR_CHANOPRIVSNEEDED(sender._nick, _name));
	else if (modes.empty())
		sender._messages.push_back(RPL_CHANNELMODEIS(sender._nick, _name, _activeMode, getValues()));
	else {
		for (size_t i = 0; i < modes.size(); i++) {
			appendPrint = false;
			sign = (modes.at(i) == '-' ? 0 : (modes.at(i) == '+' ? 1 : sign));
			if (modes.at(i) == '-' || modes.at(i) == '+')
				i++;

			if (_modes.substr(0, 3).find_first_of(modes.at(i)) != std::string::npos)
			{
				if (sign < 0)
					sender._messages.push_back(ERR_UNKNOWNERROR(sender._nick, "MODE", modes.at(i), "modestring must always precede with a sign! +/-"));
				else 
					appendPrint = handleTypeBC(sender, modes.at(i), sign, getCmd(value));
			}
			else if (_modes.substr(3, 2).find_first_of(modes.at(i)) != std::string::npos) {
				if (sign < 0)
					sender._messages.push_back(ERR_UNKNOWNERROR(sender._nick, "MODE", modes.at(i), "modestring must always precede with a sign! +/-"));
				else 
					appendPrint = handleTypeD(modes.at(i), sign);
			}
			else
				sender._messages.push_back(ERR_UNKNOWNMODE(sender._nick, modes.at(i)));

			if (appendPrint) {
				printMode += (sign ? "+" : "-") + modes.substr(i, 1);
				if (_modes.substr(0, 3).find_first_of(modes.at(i)) != std::string::npos) {
					printValue += (printValue.empty() ? "" : ",") + (!modes.substr(i, 1).compare("k") ? (sign ? "-\\(\",)/-" : "") : getCmd(value)); //hiding sensitive info!
					value = removeCmd(value);
				}
			}
		}
		for (chnMemIt it = _members.begin(); it != _members.end() && printMode.size(); it++)
			it->first->_messages.push_back(":" + sender._identifier + " MODE " + _name + " " + printMode + " " + printValue + "\r\n");
	}
}

void	Channel::seTopic(std::string author, std::string topic) {
	std::time_t currentTime; std::time(&currentTime);

	_topiCreation = (std::string)std::ctime(&currentTime);
	_topiCreation = _topiCreation.substr(0, _topiCreation.size() - 1);
	_topicAuthor = author;
	_topic = topic.empty() ? topic : topic.at(0) == ':' ? removeColon(topic) : getCmd(topic);
	for (chnMemIt it = _members.begin(); it != _members.end(); it++) {
		it->first->_messages.push_back(RPL_TOPIC(it->first->_nick, _name, _topic));
		it->first->_messages.push_back(RPL_TOPICWHOTIME(it->first->_nick, _name, _topicAuthor, _topiCreation));
	}
}

void	Channel::geTopic(Client& user) {
	if (_topic.empty())
		user._messages.push_back(RPL_NOTOPIC(user._nick, _name));
	else {
		user._messages.push_back(RPL_TOPIC(user._nick, _name, _topic));
		user._messages.push_back(RPL_TOPICWHOTIME(user._nick, _name, _topicAuthor, _topiCreation));
	}
}

bool	Channel::chkTopic() {
	return (_topic.empty() ? false : true);
}

void	Channel::relayMessage(Client &client, std::string message) {
	for (chnMemIt it = _members.begin(); it != _members.end(); it++)
		if (it->first != &client)
			it->first->_messages.push_back(message);
}

void	Channel::removeMember(Client &client, std::string message) {
	for (chnMemIt it = _members.begin(); it != _members.end(); it++)
		it->first->_messages.push_back(message);
	_members.erase(&client);
	std::vector<Channel*>::iterator	it = client._channels.begin();
	if (it != client._channels.end())
		client._channels.erase(it);
}
