#pragma once

# include <string>
# include <sys/socket.h>
# include <netdb.h>
# include <poll.h>
# include "Server.hpp"
# include <deque>
# include <vector>
# include "Commands.hpp"

class Server;
class Commands;
class Channel;

class Client
{
private:
	friend class Server; //Allows Server to access private members of Client
	friend class Commands;
	friend class Channel;

	int 						_sockfd;
	std::string					_nick;
	std::string					_username;
	std::string					_hostname;
	std::string					_server;
	std::string					_realname;
	std::string					_fullMsg;
	bool						_isBotFirstCall;
	bool						_listenSock;
	bool						_authenticated;
	bool						_registered;
	struct sockaddr_in			_clientInfo;
	std::deque<std::string>		_messages;
	std::vector<std::string>	_invitations;
	std::vector<Channel*>		_channels;
public:
	std::string					_identifier; //<nick>!<user>@<host>
	
	Client();

	void		printClient(); 
	void		printPendingMsgs();
	bool		appendBuffer(char* buf);
	bool		chkOverflow();
	std::string	getFullMsg();
};
