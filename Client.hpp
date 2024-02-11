#pragma once

# include <string>
# include <sys/socket.h>
# include <netdb.h>
# include <poll.h>
# include "Server.hpp"

class Server;

class Client
{
private:
	friend class Server;

	int 				_sockfd;
	std::string			_nick;
	std::string			_user;
	std::string			_realName;
	char				_mode; //do we need this?
	bool				_listenSock;
	bool				_authenticated;
	bool				_registered;
	struct pollfd		_pfd;
	struct sockaddr_in	_clientInfo;
	// Message				_msg;

	// Client const & operator=(Client const & src);
public:
	Client();
	// Client(Client const & src);
	Client(int fd);
	// ~Client();

	void	printClient();

	class InvalidClientConnectionException : public std::exception{
		const char *what() const throw(){return "Can't connect client";}
	};
 
};

/* NOTES:
First we must parse the command and make sure that all parameters and calls are correct
Then we can use a map to store <COMMAND, FUNCPTR>

PASS MUST be called first;
then user and nick can be called in either order

AS it turns out, in order for a container to access a class, the class needs to have a
public constructor, destructor, and copy constructor. Since the container is a class as well

*/