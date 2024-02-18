#include "Commands.hpp"

/* 
All command functions should:
1. Set info (if applicable)
2. generate a message (if applicable)
3. push message to appropriate clients
*/

//for now we will not give any capabilities to our server
void	Commands::CAP()
{
	if 	(_param[0] == "LS")
	{
		std::string msg = "CAP * LS :\r\n";
		Server::_pfdsMap[_sender]._messages.push_back(msg);
	}
	// else if (_param[0] == "REQ")
	// 	//send CAP * ACK :param[1]

}

void	Commands::PASS()
{
	if (_param[0] == Server::getpassword())
		_authenticated = true;
	else if (_param[0] != password)
	{
		std::string msg = source + ERR_PASSWDMISMATCH + _pfdsMap[_sender]._nick + ": Password is invalid\r\n";
		Server::_pfdsMap[_sender]._messages.push_back(msg);
		//should we terminate the connection here? but if we terminate it now, then we can't send the error message
	}	
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
			else (/*if client registration is set already*/)
				//msg ex: :h!~hbui-vu@127.0.0.1 NICK  hello :hello
		}
		//push message into container
	}
}

void	Commands::USER()
{
	//if not yet authenticated, send error msg
	//user, mode, and Real name
	//is nick already set, check _registered
	//if registered, send welcome message
}

void	Commands::QUIT()

void	Commands::EXIT()

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