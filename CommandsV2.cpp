#include "CommandsV2.hpp"

/* 
 thers just a small issue iwht irssi where it prints unknown command even after it processes the messge
 its not a big issue, and im just too tired and sleepy rn, I'll look into it later!
 */

typedef void (Commands::*cmdPtr)(void);
using namespace	std;// temporary! 💀, using this for cout and cerr debugs!

Commands::Commands(int fd, std::string command, std::string param, Client& sender)
	:	_senderFd(fd),
		_command(command),
		_param(param),
		_sender(sender)
		// _receiver(0)
{
	cmdPtr	ptr;
	std::string	cmds[] = {"CAP", "PASS", "NICK", "USER", "PRIVMSG"};
	size_t	cmd = 0, amtCmds = sizeof(cmds) / sizeof(std::string);
	for (; cmd < amtCmds && cmds[cmd].compare(_command); cmd++);
	switch (cmd) {
		case 0: ptr = &Commands::CAP; break;
		case 1: ptr = &Commands::PASS; break;
		case 2: ptr = &Commands::NICK; break;
		case 3: ptr = &Commands::USER; break;
		case 4: ptr = &Commands::PRIVMSG; break;
		default : ptr = &Commands::UNKNOWN;
	}
	(this->*ptr)();
	(void)_senderFd;
}

//adjust welcome message
void	Commands::WelcomeMsg()
{
	_sender._messages.push_back(RPL_WELCOME(Server::getServername(), _sender._nick, _sender._identifier));
	_sender._messages.push_back(RPL_YOURHOST(Server::getServername(), _sender._nick));
	_sender._messages.push_back(RPL_CREATED(Server::getServername(), _sender._nick));
	_sender._messages.push_back(RPL_MYINFO(Server::getServername(), _sender._nick));
	_sender._messages.push_back(RPL_ISUPPORT(Server::getServername(), _sender._nick));
}

//adjust motd
void	Commands::MOTD()
{
	_sender._messages.push_back(RPL_MOTDSTART(Server::getServername(), _sender._nick));
	_sender._messages.push_back(RPL_MOTD(Server::getServername(), _sender._nick));
	_sender._messages.push_back(RPL_ENDOFMOTD(Server::getServername(), _sender._nick));
}

void	Commands::completeRegistration(std::string nick)
{
	_sender._registered = true;
	_sender._identifier = nick + "!" + _sender._username + "@" + _sender._hostname;
	WelcomeMsg();
	MOTD();
}

void	Commands::PASS() {//complete!
	if (_param.empty())
		_sender._messages.push_back(ERR_NEEDMOREPARAMS(_command));
	else if (_sender._authenticated)
		_sender._messages.push_back(ERR_ALREADYREGISTERED(Server::getServername(), _sender._nick));
	else if (_param.compare(Server::getPassword()))
		_sender._messages.push_back(ERR_PASSWDMISMATCH(Server::getServername(), _sender._nick));
	else
		_sender._authenticated = true;
}

void	Commands::NICK() {
	std::string	invLead = ":#$&0123456789";

	if (!_sender._authenticated)
		return ;//can add ERR_NOTREGISTERED and have a custum msg each time this is used! depending on the scenario
		// _sender._messages.push_back(ERR_NEEDMOREPARAMS(_command));//pass not sup!
	else if (_param.empty())
		_sender._messages.push_back(ERR_NEEDMOREPARAMS(_command));//ERR_NONICKNAMEGIVEN
	// else if (targ_max nicklen!) //will add after we decide on nick len
	else if ((Server::_nickMap.find(_param)) != Server::_nickMap.end())
		_sender._messages.push_back(ERR_NICKNAMEINUSE(Server::getServername(), _sender._nick));
	else if ((invLead.find(_param.at(0)) != std::string::npos) || _param.find_first_of(" !@*?,.") != std::string::npos)
		_sender._messages.push_back(ERR_ERRONEUSNICKNAME(Server::getServername(), _sender._nick));
	else {
		if (_sender._registered == true) {
			Server::_nickMap.erase(_sender._nick);
			_sender._messages.push_back(NICKNAME(_sender._identifier, _param));
			_sender._identifier = _param + "!" + _sender._username + "@" + _sender._hostname;
		} else if (!_sender._username.empty()) {
			completeRegistration(_param);
		}
		_sender._nick = _param;
		Server::_nickMap[_param] = _sender._sockfd;
	}
}

void	Commands::USER()
{
	Client& _sender = Server::_pfdsMap[_senderFd];

	if (!_sender._authenticated)
		return ;// same idea as the one for NICK
	if (_sender._registered == true || !_sender._username.empty()) //but what happens if no nick
		_sender._messages.push_back(ERR_ALREADYREGISTERED(Server::getServername(), _sender._nick));
	else if (_param.size() < 4)
		_sender._messages.push_back(ERR_NEEDMOREPARAMS(_command));
	else
	{// the extraction below looks awful, but don't worry this is temporary I'll fix it later, for now it does the job!
		_sender._username = getCmd(_param);
		_sender._hostname = getCmd(removeCmd(_param));
		_sender._server = getCmd(removeCmd(removeCmd(_param)));
		_sender._realname = getCmd(removeCmd(removeCmd(removeCmd(_param))));
		if (_sender._nick != "")
			completeRegistration(_sender._nick);
	}
}

void	Commands::CAP() {//complete, gotta test this later!
	if (_param.empty())
		_sender._messages.push_back(ERR_NEEDMOREPARAMS(_command));
	else if (!_param.compare("LS") || !_param.compare("LS 302"))
		_sender._messages.push_back("CAP * LS :");
	else if (!_param.compare("LIST"))
		_sender._messages.push_back("CAP * LIST :");
	else if (!_param.compare("REQ") || !_param.compare(0, 4, "REQ "))
		_sender._messages.push_back("CAP * NAK : " + _param);
	else if (!_param.compare("END"))
		return ;
	else
		_sender._messages.push_back(ERR_INVALIDCAPCMD(Server::getServername(), _sender._nick, _command));
}

void	Commands::UNKNOWN() {
	_sender._messages.push_back(ERR_UNKNOWNCOMMAND(Server::getServername(), _sender._nick, _command));
}

//////////////////////////////////////////////// PRIVMSG //////////////////////////////////////////////////////

void	Commands::MsgChannel(void) {
	cout << "work in progress!" << endl;
}

void	Commands::MsgClient(std::string recipient, std::string text) {
	std::map<std::string, int>::iterator it = Server::_nickMap.find(recipient);
	if (it == Server::_nickMap.end())
		_sender._messages.push_back(ERR_NOSUCHNICK(Server::getServername(), _sender._nick, recipient));
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
		_sender._messages.push_back(ERR_NORECIPIENT(Server::getServername(), _sender._nick));
	else if (removeCmd(_param).empty())
		_sender._messages.push_back(ERR_NOTEXTTOSEND(Server::getServername(), _sender._nick));
	else {
		text = removeCmd(_param);
		recipients = splitPlusPlus(getCmd(_param), ",");
		for (std::vector<std::string>::iterator	it = recipients.begin(); it != recipients.end(); it++) {
			//TARGMAX privmsg, once u guys decide how many recipients I should relay the message to I'll add it here!
			if (!it->size())
				_sender._messages.push_back(ERR_NORECIPIENT(Server::getServername(), _sender._nick));
			else if (it->at(0) == '#')
				MsgChannel();
			else
				MsgClient(*it, text);
		}
	}
}