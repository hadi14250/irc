#pragma once

# include <string>
# include <sys/socket.h>
# include <netdb.h>
# include <poll.h>
# include "Server.hpp"
# include <deque>
# include "Commands.hpp"

class Server;
class Commands;

class Client
{
private:
	friend class Server; //Allows Server to access private members of Client
	friend class Commands;

	int 					_sockfd;
	std::string				_nick;
	std::string				_username;
	std::string				_hostname;
	std::string				_server;
	std::string				_realname;
	std::string				_identifier; //<nick>!<user>@<host>
	bool					_listenSock;
	bool					_authenticated;
	bool					_registered;
	struct pollfd			_pfd;
	std::deque<std::string>	_messages;
	// char					_mode; //do we need this?
	// Client const & operator=(Client const & src);
public:
	Client();
	// Client(Client const & src);;
	// ~Client();

	void	printClient(); 
	void	printPendingMsgs();
};

/* NOTES:
First we must parse the command and make sure that all parameters and calls are correct
Then we can use a map to store <COMMAND, FUNCPTR>

PASS MUST be called first;
then user and nick can be called in either order

AS it turns out, in order for a container to access a class, the class needs to have a
public constructor, destructor, and copy constructor. Since the container is a class as well

*/