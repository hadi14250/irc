#include "Commands.hpp"

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
	std::string	cmds[] = {"JOIN", "NAMES", "MODE", "TOPIC", "INVITE", "KICK", "PRIVMSG", "WHOIS", "PART", "QUIT"};
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
		case 7: ptr = &Commands::WHOIS; break;
		case 8: ptr = &Commands::PART; break;
		case 9: ptr = &Commands::QUIT; break;
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

void	Commands::WelcomeMsg()
{
	_sender._messages.push_back(RPL_WELCOME(_sender._nick, _sender._identifier));
	_sender._messages.push_back(RPL_YOURHOST(_sender._nick));
	_sender._messages.push_back(RPL_CREATED(_sender._nick));
	_sender._messages.push_back(RPL_MYINFO(_sender._nick));
	_sender._messages.push_back(RPL_ISUPPORT(_sender._nick));
}

void	Commands::MOTD()
{
	std::ifstream MOTDfile("welcome.motd");
	if (!MOTDfile.is_open())
		_sender._messages.push_back(ERR_NOMOTD(_sender._nick));
	else
	{
		_sender._messages.push_back(RPL_MOTDSTART(_sender._nick));
		std::string line;
		while ((std::getline(MOTDfile, line)))
			_sender._messages.push_back(RPL_MOTD(_sender._nick) + " :" + line + "\r\n");
		_sender._messages.push_back(RPL_ENDOFMOTD(_sender._nick));
	}
}

void	Commands::completeRegistration(std::string nick)
{
	_sender._registered = true;
	_sender._identifier = nick + "!~" + _sender._username + "@" + _sender._hostname;
	WelcomeMsg();
	MOTD();
}

void	Commands::PASS() {
	if (_param.empty())
		_sender._messages.push_back(ERR_NEEDMOREPARAMS(_sender._nick, _command));
	else if (_sender._authenticated)
		_sender._messages.push_back(ERR_ALREADYREGISTERED(_sender._nick));
	else if (_param.compare(Server::getPassword()))
		_sender._messages.push_back(ERR_PASSWDMISMATCH(_sender._nick));
	else
	{
		std::cout << GREEN << "authorized fd " << _senderFd << RESET << std::endl;
		_sender._authenticated = true;
	}
}

void	Commands::NICK() {
	std::string	invLead = "!@#$%&*()+=-;:<>,.?/0123456789";
	std::string invStr = "!@*?,.";

	if (!_sender._authenticated)
		_sender._messages.push_back(ERR_NOTREGISTERED(_sender._nick, "password not provided!"));
	else if (_param.empty())
		_sender._messages.push_back(ERR_NONICKNAMEGIVEN(_sender._nick));
	else if ((Server::_nickMap.find(_param)) != Server::_nickMap.end())
		_sender._messages.push_back(ERR_NICKNAMEINUSE(_param));
	else if ((invLead.find(_param.at(0)) != std::string::npos) || _param.find_first_of(invStr) != std::string::npos)
		_sender._messages.push_back(ERR_ERRONEUSNICKNAME(_sender._nick));
	else {
		if (_param.length() > 9)
			_param = _param.substr(0, 15);
		if (_sender._nick.compare("*")) {
			Server::_nickMap.erase(_sender._nick);
			_sender._messages.push_back(NICKNAME(_sender._nick, _param));
			for (std::vector<Channel*>::iterator it = _sender._channels.begin(); it != _sender._channels.end(); it++)
				(*it)->relayMessage(_sender, NICKNAME(_sender._identifier, _param));
		}
		Server::_nickMap[_param] = _senderFd;
		_sender._nick = _param;
		_sender._identifier = _sender._nick + "!" + _sender._username + "@" + _sender._hostname;
		if (!_sender._registered && _sender._username.size())
			completeRegistration(_sender._nick);
	}
}

void	Commands::USER()
{
	Client& _sender = Server::_pfdsMap[_senderFd];

	if (!_sender._authenticated)
		_sender._messages.push_back(ERR_NOTREGISTERED(_sender._nick, "password not provided!"));
	else if (_sender._registered)
		_sender._messages.push_back(ERR_ALREADYREGISTERED(_sender._nick));
	else if (_param.size() < 1)
		_sender._messages.push_back(ERR_NEEDMOREPARAMS(_sender._nick, _command));
	else {
		_sender._username = getCmd(_param).substr(0, USER_LEN);
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
	else if (_param.compare("LS") == 0 || _param.compare("LS 302") == 0)
		_sender._messages.push_back("CAP * LS :\r\n");
	else if (_param.compare("END") == 0)
		return ;
	else
		_sender._messages.push_back(ERR_INVALIDCAPCMD(_sender._nick, _command));
}

void Commands::WHOIS() {
	if (_param.empty())
		_sender._messages.push_back(ERR_NONICKNAMEGIVEN(_sender._nick));
	else if (Server::_nickMap.find(_param) == Server::_nickMap.end())
		_sender._messages.push_back(ERR_NOSUCHNICK(_sender._nick, _param));
	else
	{
		Client &client = Server::_pfdsMap[Server::_nickMap[_param]];
		_sender._messages.push_back(RPL_WHOISUSER(_sender._nick, client._nick, client._username, client._hostname, client._realname));
		_sender._messages.push_back(RPL_WHOISSERVER(_sender._nick, client._nick, client._server));
		_sender._messages.push_back(RPL_ENDOFWHOIS(_sender._nick));
	}
}

void	Commands::MsgClient(std::string recipient, std::string text) {
	std::map<std::string, int>::iterator it = Server::_nickMap.find(recipient);
	if (it == Server::_nickMap.end())
		_sender._messages.push_back(ERR_NOSUCHNICK(_sender._nick, recipient));
	else	
	{
		if (text.find("#SASSYBOT") != std::string::npos){
			text = BOT();
		} 
		else if (text.size() && text.at(0) == ':')
		{
			int textLen = text.size();
			text = text.substr(1, textLen - 1);
		}
 		Server::_pfdsMap[it->second]._messages.push_back(PRIV_MSG(_sender._identifier, recipient, text));
	}
}

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
			if (!it->size())
				_sender._messages.push_back(ERR_NORECIPIENT(_sender._nick));
			else if (it->at(0) == '#') {
				if ( Server::_chanMap.find(*it) == Server::_chanMap.end())
					_sender._messages.push_back(ERR_NOSUCHCHANNEL(_sender._nick, *it));
				else
					Server::_chanMap[*it].msgChannel(_sender, text);
			} else
				MsgClient(*it, text);
		}
	}
}

void	Commands::NAMES() {
	if (_param.empty())
		for (chnMapIt it = Server::_chanMap.begin(); it != Server::_chanMap.end(); it++)
			_param += (it != Server::_chanMap.begin() ? ("," + it->first) : it->first);
	std::vector<std::string>	message;
	std::vector<std::string>	channels = splitPlusPlus(_param, ",");
	for (vecStrIt it = channels.begin(); it != channels.end(); it++) {
		if (Server::_chanMap.find(*it) == Server::_chanMap.end()) {
			_sender._messages.push_back(RPL_ENDOFNAMES(_sender._nick, *it));
			continue ;
		}
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
	else if (!_param.compare("0")) {
		_command = "PART";
		_param.clear();
		for (std::vector<Channel*>::iterator it = _sender._channels.begin(); it != _sender._channels.end(); it++) {
			_param += (_param.empty() ? "" : ",") + (*it)->getChannelName();
		}
		PART();
	} else {
		if ((channels = splitPlusPlus(getCmd(_param), ",")).empty())
			_sender._messages.push_back(ERR_BADCHANMASK(_sender._nick, "", "channel Name not provided!"));
		keys = splitPlusPlus(getCmd(removeCmd(_param)), ",");
		for (vecStrIt it = channels.begin(); it != channels.end(); it++) {
			std::string	key = i >= keys.size() ? "" : keys.at(i);
			if (Server::_chanMap.find(*it) == Server::_chanMap.end()) {
				if (it->size() > CHAN_LEN)
				{
					_sender._messages.push_back(ERR_BADCHANNAME(_sender._nick, *it));
					continue ;
				}
				else if (it->size() && (it->at(0) == '#' || it->at(0) == '&') && ((it->find_first_of(" ^") == std::string::npos) ) )
					Channel	newChan(*it);
				else {
					_sender._messages.push_back(ERR_BADCHANMASK(_sender._nick, *it, "Bad Channel Mask"));
					continue ;
				}
			}
			if (Server::_chanMap[*it].joinChannel(_sender, key)) {
				_param = *it;
				if (Server::_chanMap[*it].chkTopic())
					Server::_chanMap[*it].geTopic(_sender);
				NAMES();
			}
		}
	}
}

void	Commands::MODE() {
	if (_param.empty() || (chkArgs(_param, 1) < 1))
		_sender._messages.push_back(ERR_NEEDMOREPARAMS(_sender._nick, _command));
	else if (_param.at(0) == '#') {
		chnMapIt	it = Server::_chanMap.find(getCmd(_param));
		_param = removeCmd(_param);
		if (it != Server::_chanMap.end())
			it->second.chanMode(_sender, getCmd(_param), removeCmd(_param));
		else
			_sender._messages.push_back(ERR_NOSUCHCHANNEL(_sender._nick, getCmd(_param)));
	} else
		_sender._messages.push_back(ERR_UMODEUNKNOWNFLAG(_sender._nick));
}

void	Commands::TOPIC() {
	chnMapIt	it = Server::_chanMap.find(getCmd(_param));

	if (_param.empty())
		_sender._messages.push_back(ERR_NEEDMOREPARAMS(_sender._nick, _command));
	else if (it == Server::_chanMap.end())
		_sender._messages.push_back(ERR_NOSUCHCHANNEL(_sender._nick, getCmd(_param)));
	else if (!it->second.chkIfMember(_sender._nick))
		_sender._messages.push_back(ERR_NOTONCHANNEL(_sender._nick, getCmd(_param)));
	else if (it->second.chkTopicFlag() && !it->second.chkIfOper(_sender._nick))
		_sender._messages.push_back(ERR_CHANOPRIVSNEEDED(_sender._nick, getCmd(_param)));
	else
		(!chkArgs(removeCmd(_param), 1)) ? it->second.geTopic(_sender) : it->second.seTopic(_sender._nick, removeCmd(_param).substr(0, TOPC_LEN));
}

void	Commands::INVITE() {
	if (_param.empty() || (chkArgs(_param, 2) < 2)) {
		_sender._messages.push_back(ERR_NEEDMOREPARAMS(_sender._nick, _command));
		return ;
	}
	chnMapIt	it;
	std::string	channelName = getCmd(removeCmd(_param));
	std::string	recipient = getCmd(_param);
	if ((it = Server::_chanMap.find(channelName)) == Server::_chanMap.end())
		_sender._messages.push_back(ERR_NOSUCHCHANNEL(_sender._nick, channelName));
	else if (Server::_nickMap.find(recipient) == Server::_nickMap.end())
		_sender._messages.push_back(ERR_NOSUCHNICK(_sender._nick, recipient));
	else if (!it->second.chkIfMember(_sender._nick))
		_sender._messages.push_back(ERR_NOTONCHANNEL(_sender._nick, channelName));
	else if (it->second.chkIfMember(recipient))
		_sender._messages.push_back(ERR_USERONCHANNEL(_sender._nick, recipient, channelName));
	else if (!it->second.chkIfOper(_sender._nick))
		_sender._messages.push_back(ERR_CHANOPRIVSNEEDED(_sender._nick, channelName));
	else {
		_sender._messages.push_back(RPL_INVITING(_sender._nick, getCmd(removeCmd(_param)), getCmd(_param)));
		Server::_pfdsMap[Server::_nickMap[recipient]]._messages.push_back(INVITE_MSG(_sender._identifier, getCmd(removeCmd(_param)), getCmd(_param)));
		Server::_pfdsMap[Server::_nickMap[recipient]]._invitations.push_back(channelName);
	}
}

void	Commands::KICK() {
	if (chkArgs(_param, 2) < 2)
	{
		_sender._messages.push_back(ERR_NEEDMOREPARAMS(_sender._nick, _command));
		return;
	}

	std::vector<std::string>	channels = splitPlusPlus(getCmd(_param), ",");
	std::vector<std::string>	victims = splitPlusPlus(getCmd(removeCmd(_param)), ",");
	std::string					kickMsg = (chkArgs(_param, 3) < 3) ? "removing user!" : removeCmd(removeCmd(_param)).substr(0, KICK_LEN);

	chnMapIt	mapIt;
	vecStrIt	chnIt = channels.begin();

	if (chnIt->empty() || (chnIt->at(0) != '#'))
		_sender._messages.push_back(ERR_BADCHANMASK(_sender._nick, *chnIt, "provided channel names isn't valid!"));
	else if ((mapIt = Server::_chanMap.find(*chnIt)) == Server::_chanMap.end())
		_sender._messages.push_back(ERR_NOSUCHCHANNEL(_sender._nick, *chnIt));
	else if (!mapIt->second.chkIfMember(_sender._nick))
		_sender._messages.push_back(ERR_NOTONCHANNEL(_sender._nick, *chnIt));
	else if (!mapIt->second.chkIfOper(_sender._nick))
		_sender._messages.push_back(ERR_CHANOPRIVSNEEDED(_sender._nick, *chnIt));
	else{
		for (vecStrIt	vicIt = victims.begin(); vicIt!= victims.end(); vicIt++ ){
			if (!mapIt->second.chkIfMember(*vicIt) && _sender._nick.compare(*vicIt))
				_sender._messages.push_back(ERR_USERNOTINCHANNEL(_sender._nick, *vicIt, *chnIt));
			else if (_sender._nick.compare(*vicIt))
				mapIt->second.removeMember(Server::_pfdsMap[Server::_nickMap[*vicIt]], KICK_MSG(_sender._identifier, *chnIt, *vicIt, kickMsg));
		}
	}
}

void	Commands::PART() {
	chnMapIt	mapIt;

	if (!chkArgs(_param, 1))
		_sender._messages.push_back(ERR_NEEDMOREPARAMS(_sender._nick, _command));
	else {
		std::vector<std::string> channels = splitPlusPlus(_param, ",");

		for (vecStrIt it = channels.begin(); it != channels.end(); it++) {
			mapIt = Server::_chanMap.find(*it);
			if (mapIt == Server::_chanMap.end())
				_sender._messages.push_back(ERR_NOSUCHCHANNEL(_sender._nick, *it));
			else if (!mapIt->second.chkIfMember(_sender._nick))
				_sender._messages.push_back(ERR_NOTONCHANNEL(_sender._nick, *it));
			else
			{
				mapIt->second.removeMember(_sender, PART_MSG(_sender._identifier, *it));
				if (mapIt->second._members.empty())
					Server::_chanMap.erase(*it);
			}
		}

	}
}

void	Commands::QUIT()
{
	Server::deletePfd(_senderFd);
}

std::string	Commands::BOT() {
	std::string reply;
	if (_sender._isBotFirstCall == true) {
		reply = Bot::botName + ": " +  Bot::popUpResponse;
		_sender._isBotFirstCall = false;
	}
	else if (_sender._isBotFirstCall == false) {
		std::srand(static_cast<unsigned int>(std::time(nullptr)));
	    int strSize = sizeof(Bot::helpResponse) / sizeof(Bot::helpResponse[0]);
		int randIdx = std::rand() % strSize;
	    const std::string& response = Bot::helpResponse[randIdx];
		reply = Bot::botName + ": " + response;
	}
	return reply;
}
