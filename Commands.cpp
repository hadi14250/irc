#include "Commands.hpp"

//for now we will not give any capabilities to our server
void	Commands::CAP()
{
	if 	(_param.front() == "LS")
		_sender._messages.push_back("CAP * LS :\r\n");
	// else if (_param[0] == "REQ")
	// 	//send CAP * ACK :param[1]

}

void	Commands::PASS()
{
	if (_param.empty())
		_sender._messages.push_back(ERR_NEEDMOREPARAMS(_command));
	else if (_sender._authenticated == true)
		_sender._messages.push_back(ERR_ALREADYREGISTERED(Server::getServername(), _sender._nick));
	else if (_param.front() != Server::getPassword())
		_sender._messages.push_back(ERR_PASSWDMISMATCH(Server::getServername(), _sender._nick));
	else
		_sender._authenticated = true;
	//should we terminate the connction if there is passowrd error? if we do, we have to send ERROR command	
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

void	Commands::completeRegistration()
{
	_sender._registered = true;
	_sender._identifier = _sender._nick + "!" + _sender._username + "@" + _sender._hostname;
	WelcomeMsg();
	MOTD();
	Server::_nickMap[_sender._nick] = _sender._pfd.fd;
}

bool	Commands::invalidNick()
{
	char c = _param[0][0];
	if (c == '#' || c == '&' || c == ':' || _param[0].find_first_of(" ") != std::string::npos)
		return true;
	return false;
}


void	Commands::NICK()
{
	if (_sender._authenticated == false)
		return ;
	if (_param.empty())
		_sender._messages.push_back(ERR_NONICKNAMEGIVEN(Server::getServername(), _sender._nick));
	else
	{
		std::map<std::string, int>::iterator it = Server::_nickMap.find(_param[0]);
		if (it != Server::_nickMap.end())
			_sender._messages.push_back(ERR_NICKNAMEINUSE(Server::getServername(), _sender._nick));
		else if (invalidNick())
			_sender._messages.push_back(ERR_ERRONEUSNICKNAME(Server::getServername(), _sender._nick));
		else
		{
			_sender._nick = _param[0];
			if (_sender._registered == true)
			{
				_sender._messages.push_back(NICKNAME(_sender._identifier, _sender._nick));
				_sender._identifier = _sender._nick + "!" + _sender._username + "@" + _sender._hostname;
			}
			else if (!_sender._username.empty())
				completeRegistration();
		}
	}
}

/* USER username hostname servername(of user) :<realname(can contain spaces)*/
void	Commands::USER()
{
	Client& _sender = Server::_pfdsMap[_fd];

	if (_sender._authenticated == false)
		return ;
	if (_sender._registered == true || !_sender._username.empty()) //but what happens if no nick
		_sender._messages.push_back(ERR_ALREADYREGISTERED(Server::getServername(), _sender._nick));
	else if (_param.size() < 4)
		_sender._messages.push_back(ERR_NEEDMOREPARAMS(_command));
	else
	{
		_sender._username = _param[0];
		_sender._hostname = _param[1];
		_sender._server = _param[2];
		_sender._realname = _param[3];
		if (_sender._nick != "")
			completeRegistration();
	}
}


// void	Commands::QUIT()

// void	Commands::EXIT()

/*
<type> <command> <code> [<context>...] <description>
type: FAIL, WARN, or NOTE, 
command: Indicates the user command which this reply is related to, or is * for messages initiated outside _sender commands (for example, an on-connect message).
code: Machine-readable reply code representing the meaning of the message to _sender software.
context: Optional parameters that give humans extra context as to where and why the reply was spawned (for example, a particular subcommand or sub-process).
description:  required plain-text description of the reply which is shown to users.
*/
// std::string generateFailMessage(std::string type, std::string command, std::string code)
// {
// 	std::string msg;
// 	if (type == "FAIL")
// 	{
// 		msg = type + " " + command + " " + code;
// 		if (code == "NEED_REGISTRATION")
// 			msg += " :You are not yet registered\r\n";
// 	}
// 	return (msg);
// }

/* NOTES:
CAP = _sender capability negotiation
allows IRC clients and servers to negotiate new features in a backwards-compatible way
-basically allows _sender and server to operate with the same version of capabilities (protocol extensions)
1. upon connection, _sender will send server a CAP message to start negotiation
	-CAP LS [version] - to discover available capabilities on server
	or 
	-CAP REQ - to blindly request a set of capabilities

Common capabilities:
1. multi-prefix: 
This capability allows clients to receive more detailed user mode information for each user in channel membership 
lists (such as NAMES responses). Without multi-prefix, clients receive only the highest user mode for each user, 
which is typically a single-character mode like @ (for operator) or + (for voice). With multi-prefix, clients 
can receive a list of all modes for each user, allowing for more granular control and display of user permissions.

2. userhost-in-names: 
This capability allows clients to include the full userhost information (nickname!username@hostname) in NAMES list 
responses. Without this capability, clients only receive nicknames in NAMES responses, and they would need to send 
additional WHO commands to retrieve userhost information for each nickname. Enabling userhost-in-names reduces the 
number of required WHO commands and can improve _sender performance.

3. sasl: 
SASL (Simple Authentication and Security Layer) is a method for authentication between clients and servers. The sasl 
capability enables clients to authenticate using SASL mechanisms, such as PLAIN or EXTERNAL, which provide a more 
secure authentication method than the traditional IRC PASS command. Enabling the sasl capability allows clients to 
authenticate securely before joining channels or sending messages.

2. Client then must send standard nick and user to complete registration
3. Server use 
*/

/* DRAFTS:

void	Commands::generateMessage(std::string src, int code, Client &_sender)
{
	std::string paramMsg;
	switch(code) //can't use switch with strings...maybe make code a number and then convert to string
	{
		case RPL_WELCOME: 
			paramMsg = "Welcome to ft-irc " + _sender._identifier + "!"; break;
		case ERR_NEEDMOREPARAMS:
			paramMsg = "More parameters needed"; break; //we may need to add comand to this too
			//"<_sender> <command> :Not enough parameters"
		case ERR_ALREADYREGISTERED:
			paramMsg = "You are already registered"; break;
		case ERR_PASSWDMISMATCH
			paramMsg = "Invalid password"; break;
		
	}
	std::string codeStr = //convert code to string
	std::string msg = ":" + src + " " + code + " " + _sender._nick + " :" + ParamMsg + "\r\n";
	_sender._messages.push_back(msg);
}*/