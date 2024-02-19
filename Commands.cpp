#include "Commands.hpp"

/* 
class Message
{
public:
	std::string 				_command;
	std::vector<std::string>	_param;
	int							_fd;
	std::string					_sender;
	std::string					_recevier;
}
*/

/* 
All command functions should:
1. Set info (if applicable)
2. generate a message (if applicable)
3. push message to appropriate clients
*/

//for now we will not give any capabilities to our server
void	Commands::CAP()
{
	if 	(_param.front() == "LS")
	{
		std::string msg = "CAP * LS :\r\n";
		Client& client = Server::_pfdsMap[_fd];
		client._messages.push_back(msg);
	}
	// else if (_param[0] == "REQ")
	// 	//send CAP * ACK :param[1]

}

void	Commands::PASS()
{
	//if pass is not correct -> ERR_PASSWDMISMATCH
	//if pass is not present -> ERR_NEEDMOREPARAMS
	//if already authenticated -> ERR_ALREADYREGISTERED
	Client& client = Server::_pfdsMap[_fd];
	if (_param.empty())
		generateMessage(Server::getHostname(), ERR_NEEDMOREPARAMS, client);
	else if (client._authenticated == true)
		generateMessage(Server::getHostname(), ERR_ALREADYREGISTERED, client);
	else if (_param.front() != Server::getPassword())
		generateMessage(Server::getHostname(), ERR_PASSWDMISMATCH, client);
	else
		client._authenticated = true;
	//should we terminate the connction if there is passowrd error? if we do, we have to send ERROR command	
}

/*
check for:
-if nick is in use
-if nick is invalid (no leading #, no leading " ", no leading :)*/
void	Commands::NICK(Client client)
{
	if (Server::_pfdsMap[_sender]._authenticated == false)
		return ;
	std::string msg;
	if (/*param is empty*/)
		msg = src + ERR_NONICKNAMEGIVEN + _nick + ":no nickname given\r\n";
	else
	{
		std::map<int, Client>::iterator it = Server::_pfdsMap.find(_param[0]);
		if (it != Server::_pfdsMap.end())
			msg = src + ERR_NICKNAMEINUSE + _nick + ":nickname already taken. Please choose another nickname\r\n";
		else if (/*check for invalid nickname format*/)
			msg = src + ERR_ERRONEUSNICKNAME + _nick + ":invalid nickname\r\n";
		else
		{
			//set client _nick
			if (/* client registration is false && user has already been filled out*/)
				//set registration to true
				//msg = welcome msg
				//add to nicklookup
			else (/*if client registration is set already*/)
				//msg ex: :h!~hbui-vu@127.0.0.1 NICK  hello :hello
		}
		//push message into container
	}
}

/* USER username hostname servername(of user) :<realname(can contain spaces)*/
void	Commands::USER()
{
	Client& client = Server::_pfdsMap[_fd];

	if (client._authenticated == false)
		return ;
	if (client._registered == true || !client._username.empty()) //but what happens if no nick
		generateMessage(Server::getHostname(), ERR_ALREADYREGISTERED, client);
	else if (_param.size() < 4)
		//ERR_NEEDMOREPARAMS
	else
	{
		client._username = _param[0];
		client._hostname = _param[1];
		client._server = _param[2];
		client._realname = _param[3];
		if (_nick != "")
			completeRegistration(client);
	}
	if (!msg.empty())
		client._messages.push_back(msg);
}

void	Commands::completeRegistration(Client& client)
{
	client._registered = true;
	client._identifier = client._nick + "!" + client._username + "@" + client._hostname;
	makeMessage(Server::getHostname(), RPL_WELCOME, client)
	Server::nickMap[_nick] = client._pfd;
}

void	Commands::generateMessage(std::string src, int code, Client &client)
{
	std::string paramMsg;
	switch(code) //can't use switch with strings...maybe make code a number and then convert to string
	{
		case RPL_WELCOME: 
			paramMsg = "Welcome to ft-irc " + client._identifier + "!"; break;
		case ERR_NEEDMOREPARAMS:
			paramMsg = "More parameters needed"; break; //we may need to add comand to this too
			//"<client> <command> :Not enough parameters"
		case ERR_ALREADYREGISTERED:
			paramMsg = "You are already registered"; break;
		case ERR_PASSWDMISMATCH
			paramMsg = "Invalid password"; break;
		
	}
	std::string codeStr = //convert code to string
	std::string msg = ":" + src + " " + code + " " + client._nick + " :" + ParamMsg + "\r\n";
	client._messages.push_back(msg);
}

// void	Commands::QUIT()

// void	Commands::EXIT()

/*
<type> <command> <code> [<context>...] <description>
type: FAIL, WARN, or NOTE, 
command: Indicates the user command which this reply is related to, or is * for messages initiated outside client commands (for example, an on-connect message).
code: Machine-readable reply code representing the meaning of the message to client software.
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
CAP = client capability negotiation
allows IRC clients and servers to negotiate new features in a backwards-compatible way
-basically allows client and server to operate with the same version of capabilities (protocol extensions)
1. upon connection, client will send server a CAP message to start negotiation
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
number of required WHO commands and can improve client performance.

3. sasl: 
SASL (Simple Authentication and Security Layer) is a method for authentication between clients and servers. The sasl 
capability enables clients to authenticate using SASL mechanisms, such as PLAIN or EXTERNAL, which provide a more 
secure authentication method than the traditional IRC PASS command. Enabling the sasl capability allows clients to 
authenticate securely before joining channels or sending messages.

2. Client then must send standard nick and user to complete registration
3. Server use 
*/