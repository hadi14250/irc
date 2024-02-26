#include "Commands.hpp"
/* 
 thers just a small issue iwht irssi where it prints unknown command even after it processes the messge
 its not a big issue, and im just too tired and sleepy rn, I'll look into it later!
 */

using namespace	std;// temporary

Commands::Commands(int fd, std::string command, std::string param, Client& sender)
	:	_senderFd(fd),
		_command(command),
		_param(param),
		_sender(sender)
{
	if (command.empty())
		return ;
	if (!registrationCmds()) // returns true if cmd was a reg cmd or if user isn't reg! else false which means chk postReg!
		postRegistrationCmds();
}

void	Commands::postRegistrationCmds() {
	cmdPtr	ptr;
	std::string	cmds[] = {"JOIN", "NAMES", "MODE", "TOPIC", "INVITE", "KICK", "PRIVMSG", "printChan"};
	size_t	cmd = 0, amtCmds = sizeof(cmds) / sizeof(std::string);
	for (; cmd < amtCmds && cmds[cmd].compare(_command); cmd++);
	switch (cmd) {
		case 0: ptr = &Commands::JOIN; break;
		case 1: ptr = &Commands::NAMES; break;
		case 2: ptr = &Commands::MODE; break;
		case 3: ptr = &Commands::TOPIC; break;
		case 4: ptr = &Commands::INVITE; break;
		case 5: ptr = &Commands::KICK; break;
		case 6: ptr = &Commands::PRIVMSG; break;
		case 7: ptr = &Commands::printChan; break;
		default :
		_sender._messages.push_back(ERR_UNKNOWNCOMMAND(_sender._nick, _command));
		return ;
	}
	(this->*ptr)();
}

bool	Commands::registrationCmds() {
	cmdPtr	ptr;
	std::string	preRegCmds[] = {"CAP", "PASS", "NICK", "USER", "PING"};
	size_t	cmd, amtPreRegCmds = sizeof(preRegCmds) / sizeof(std::string);
	for (cmd = 0; cmd < amtPreRegCmds && preRegCmds[cmd].compare(_command); cmd++);
	switch (cmd) {
		case 0: ptr = &Commands::CAP; break;
		case 1: ptr = &Commands::PASS; break;
		case 2: ptr = &Commands::NICK; break;
		case 3: ptr = &Commands::USER; break;
		case 4: ptr = &Commands::PING; break;
		default:
		if (!_sender._registered) {
			_sender._messages.push_back(ERR_NOTREGISTERED(_sender._nick, "You have not registered!"));
			return true;
		}
	}
	if (cmd < 5)
		(this->*ptr)();
	return (cmd > 4 ? false : true);
}

void	Commands::PING() {
	if (getCmd(_param).empty())
		_sender._messages.push_back(ERR_NEEDMOREPARAMS(_sender._nick, _command));
	else
		_sender._messages.push_back(PONG(removeColon(_param)));
}

//adjust welcome message
void	Commands::WelcomeMsg()
{
	_sender._messages.push_back(RPL_WELCOME(_sender._nick, _sender._identifier));
	_sender._messages.push_back(RPL_YOURHOST(_sender._nick));
	_sender._messages.push_back(RPL_CREATED(_sender._nick));
	_sender._messages.push_back(RPL_MYINFO(_sender._nick));
	_sender._messages.push_back(RPL_ISUPPORT(_sender._nick));
}

//adjust motd
void	Commands::MOTD()
{
	_sender._messages.push_back(RPL_MOTDSTART(_sender._nick));
	_sender._messages.push_back(RPL_MOTD(_sender._nick));
	_sender._messages.push_back(RPL_ENDOFMOTD(_sender._nick));
}

void	Commands::completeRegistration(std::string nick)
{
	_sender._registered = true;
	_sender._identifier = nick + "!~" + _sender._username + "@" + _sender._hostname;
	WelcomeMsg();
	MOTD();
}

void	Commands::PASS() {//complete!
	if (_param.empty())
		_sender._messages.push_back(ERR_NEEDMOREPARAMS(_sender._nick, _command));
	else if (_sender._authenticated)
		_sender._messages.push_back(ERR_ALREADYREGISTERED(_sender._nick));
	else if (_param.compare(Server::getPassword()))
		_sender._messages.push_back(ERR_PASSWDMISMATCH(_sender._nick));
	else
		_sender._authenticated = true;
}

void	Commands::NICK() {
	std::string	invLead = ":#$&0123456789";

	if (!_sender._authenticated)
		_sender._messages.push_back(ERR_NOTREGISTERED(_sender._nick, "password not provided!"));
	else if (_param.empty())
		_sender._messages.push_back(ERR_NEEDMOREPARAMS(_sender._nick, _command));//ERR_NONICKNAMEGIVEN
	else if ((Server::_nickMap.find(_param)) != Server::_nickMap.end())
		_sender._messages.push_back(ERR_NICKNAMEINUSE(_param));
	else if ((invLead.find(_param.at(0)) != std::string::npos) || _param.find_first_of(" !@*?,.") != std::string::npos)
		_sender._messages.push_back(ERR_ERRONEUSNICKNAME(_sender._nick));
	else {
		if (_sender._registered == true) {
			Server::_nickMap.erase(_sender._nick);
			_sender._messages.push_back(NICKNAME(_sender._identifier, _param));
			_sender._identifier = _param + "!" + _sender._username + "@" + _sender._hostname;
		} else if (!_sender._username.empty()) {
			completeRegistration(_param);
		}
		_sender._nick = _param;
		Server::_nickMap[_param] = _senderFd;
	}
}

void	Commands::USER()
{
	Client& _sender = Server::_pfdsMap[_senderFd];

	if (!_sender._authenticated)
		_sender._messages.push_back(ERR_NOTREGISTERED(_sender._nick, "password not provided!"));
	if (_sender._registered == true)
		_sender._messages.push_back(ERR_ALREADYREGISTERED(_sender._nick));
	else if (_param.size() < 4)// check this
		_sender._messages.push_back(ERR_NEEDMOREPARAMS(_sender._nick, _command));
	else {
		_sender._username = getCmd(_param);
		_sender._hostname = getCmd(removeCmd(_param));
		_sender._server = getCmd(removeCmd(removeCmd(_param)));
		_sender._realname = getCmd(removeCmd(removeCmd(removeCmd(_param))));
		if (_sender._nick.compare("*"))
			completeRegistration(_sender._nick);
	}
}

void	Commands::CAP() {
	if (_param.empty())
		_sender._messages.push_back(ERR_NEEDMOREPARAMS(_sender._nick, _command));
	else if (!_param.compare("LS") || !_param.compare("LS 302"))
		_sender._messages.push_back("CAP * LS :");
	else if (!_param.compare("LIST"))
		_sender._messages.push_back("CAP * LIST :");
	else if (!_param.compare("END"))
		return ;
	else
		_sender._messages.push_back(ERR_INVALIDCAPCMD(_sender._nick, _command));
}

void	Commands::MsgClient(std::string recipient, std::string text) {
	std::map<std::string, int>::iterator it = Server::_nickMap.find(recipient);
	if (it == Server::_nickMap.end())
		_sender._messages.push_back(ERR_NOSUCHNICK(_sender._nick, recipient));
	else 
		Server::_pfdsMap[it->second]._messages.push_back(text + "\r\n");
}

/* 
	first we look for either empty recipients or text, recipients are seperated by commas and
	then theres the text which is seperated by a space from the recipients!
	for now we don't have the channel class and its implementation, I am working on it and by tommorow I will
	push a base channel implementation!
	for now it just handles client to client communication!
 */
void	Commands::PRIVMSG() {
	std::vector<std::string>	recipients;
	std::string					text;

	if (_param.empty())
		_sender._messages.push_back(ERR_NORECIPIENT(_sender._nick));
	else if (removeCmd(_param).empty())
		_sender._messages.push_back(ERR_NOTEXTTOSEND(_sender._nick));
	else {
		text = removeCmd(_param);
		recipients = splitPlusPlus(getCmd(_param), ",");
		for (std::vector<std::string>::iterator	it = recipients.begin(); it != recipients.end(); it++) {
			//TARGMAX privmsg, once u guys decide how many recipients I should relay the message to I'll add it here!
			if (!it->size())
				_sender._messages.push_back(ERR_NORECIPIENT(_sender._nick));
			else if (it->at(0) == '#' && Server::_chanMap.find(*it) != Server::_chanMap.end())
				Server::_chanMap[*it].msgChannel(_sender, text);
			else if (it->at(0) == '#')
				cout << "err";
			else
				MsgClient(*it, text);
		}
	}
}

void	Commands::NAMES() {
	for (chnMapIt it = Server::_chanMap.begin(); _param.empty() && (it != Server::_chanMap.end()); it++)
		_param += (it != Server::_chanMap.begin() ? ("," + it->first) : it->first);
	std::vector<std::string>	message;
	std::vector<std::string>	channels = splitPlusPlus(_param, ",");
	for (vecStrIt it = channels.begin(); it != channels.end(); it++) {
		if (Server::_chanMap.find(*it) == Server::_chanMap.end())
			continue ;
		message = Server::_chanMap[*it].getChannelMembers();
		for (vecStrIt msgIt = message.begin(); msgIt != message.end(); msgIt++)
			_sender._messages.push_back(RPL_NAMREPLY(_sender._nick, *it, *msgIt));
		_sender._messages.push_back(RPL_ENDOFNAMES(_sender._nick, *it));
	}
}

void	Commands::JOIN() {
	std::vector<std::string>	channels;
	std::vector<std::string>	keys;
	size_t						i = 0;

	if (!chkArgs(_param, 0))
		_sender._messages.push_back(ERR_NEEDMOREPARAMS(_sender._nick, _command));
	else if (!_param.compare("0"))// should remove user from all channels user is a part of!
		cout << "no more chann!";
	else {
		if ((channels = splitPlusPlus(_param, ",")).empty())
			_sender._messages.push_back(ERR_BADCHANMASK(_sender._nick, "", "channel Name not provided!"));
		keys = splitPlusPlus(getCmd(removeCmd(_param)), ",");
		for (vecStrIt it = channels.begin(); it != channels.end(); it++) {
			std::string	key = i >= keys.size() ? "" : keys.at(i);
			if (Server::_chanMap.find(*it) == Server::_chanMap.end()) {
				if (it->size() && it->at(0) == '#' && (it->find_first_of(" ^") == std::string::npos)/* && *it.size() > MAX_CHANNEL_NAME_LEN */)
					Channel	newChan(*it);
				else {
					_sender._messages.push_back(ERR_BADCHANMASK(_sender._nick, *it, "Bad Channel Mask"));
					continue ;
				}
			}
			if (Server::_chanMap[*it].joinChannel(_sender, key)) {
				_param = *it;	NAMES();
			}
		}
	}
}

void	Commands::MODE() {
	if (_param.empty())
		return ;// return error
	else if (_param.at(0) == '#') {
		chnMapIt	it = Server::_chanMap.find(getCmd(_param));
		_param = removeCmd(_param);
		if (it != Server::_chanMap.end())
			it->second.chanMode(_sender, getCmd(_param), removeCmd(_param));
		else
			_sender._messages.push_back("no such chan");//ERR_NOSUCHCHANNEL
	} else {
		cout << "user mode" << endl;
	}
}

void	Commands::TOPIC() {
	chnMapIt	it;
	if (_param.empty())
		_sender._messages.push_back(ERR_NEEDMOREPARAMS(_sender._nick, _command));
	else if ((it = Server::_chanMap.find(getCmd(_param))) == Server::_chanMap.end())
		_sender._messages.push_back(ERR_NOSUCHCHANNEL(_sender._nick, getCmd(_param)));
	else if (!it->second.chkIfMember(_sender._nick))
		_sender._messages.push_back(ERR_NOTONCHANNEL(_sender._nick, getCmd(_param)));
	else if (!it->second.chkIfOper(_sender._nick))
		_sender._messages.push_back(ERR_CHANOPRIVSNEEDED(_sender._nick, getCmd(_param)));
	else {
		std::vector<std::string>	topic = it->second.geTopicAndTopiCreation();
		if (!chkArgs(removeCmd(_param), 1)) {
			if (topic.at(0).empty())
				_sender._messages.push_back(RPL_NOTOPIC(_sender._nick, getCmd(_param)));
			else {
				_sender._messages.push_back(RPL_TOPIC(_sender._nick, getCmd(_param), topic.at(0) ));
				_sender._messages.push_back(RPL_TOPICWHOTIME(_sender._nick, getCmd(_param), topic.at(1), topic.at(2) ));
			}
		} else {
			it->second.seTopic(removeColon(removeCmd(_param)), _sender._nick);
			_sender._messages.push_back(RPL_TOPIC(_sender._nick, getCmd(_param), topic.at(0)));
			_sender._messages.push_back(RPL_TOPICWHOTIME(_sender._nick, getCmd(_param), topic.at(1), topic.at(2) ));
		}
	}
}

void	Commands::INVITE() {
	chnMapIt	it;
	if (_param.empty() || (chkArgs(_param, 2) < 2))
		_sender._messages.push_back(ERR_NEEDMOREPARAMS(_sender._nick, _command));
	else if (Server::_chanMap.find(getCmd(_param)) == Server::_chanMap.end())
		_sender._messages.push_back(ERR_NOSUCHCHANNEL(_sender._nick, getCmd(_param)));
	else if (Server::_nickMap.find(getCmd(removeCmd(_param))) == Server::_nickMap.end())
		_sender._messages.push_back(ERR_NOSUCHNICK(_sender._nick, getCmd(removeCmd(_param))));
	else if (!it->second.chkIfMember(_sender._nick))
		_sender._messages.push_back(ERR_NOTONCHANNEL(_sender._nick, getCmd(_param)));
	else if (it->second.chkIfMember(getCmd(removeCmd(_param))))
		_sender._messages.push_back(ERR_USERONCHANNEL(_sender._nick, getCmd(removeCmd(_param)), getCmd(_param)));
	else if (!it->second.chkIfOper(_sender._nick))
		_sender._messages.push_back(ERR_CHANOPRIVSNEEDED(_sender._nick, getCmd(_param)));
	else {
		_sender._messages.push_back(RPL_INVITING(_sender._nick, getCmd(removeCmd(_param)), getCmd(_param)));
		Server::_pfdsMap[Server::_nickMap[getCmd(removeCmd(_param))]]._messages.push_back(INVITE_MSG(_sender._identifier, getCmd(removeCmd(_param)), getCmd(_param)));
	}
}

void	Commands::KICK() {
	cerr << "wip" << endl;
}

//! tmp

void	Commands::printChan() {
chnMapIt it;
	if ((it = Server::_chanMap.find(_param)) == Server::_chanMap.end())
		return ;
	it->second.printChan();
}
