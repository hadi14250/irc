#pragma once

# include <string>
# include <sys/socket.h>
# include <netdh.h>

class Client
{
private:
	int 				_sockfd;
	std::string			_nick;
	std::string			_user;
	bool				_authenticated;
	bool				_registered;
	struct pollfd		_pfdInfo;
	struct sockaddr_in	_clientInfo;

	Client();
	Client(Client const & src);
	Client const & operator=(Client const & src);
public:
	Client(int fd);
	~Client();

	class InvalidClientConnectionException : public std::exception{
		const char *what() const throw(){return "Can't connect client";}
	}
};

/* NOTES:
First we must parse the command and make sure that all parameters and calls are correct
Then we can use a map to store <COMMAND, FUNCPTR>

PASS MUST be called first;
then user and nick can be called in either order

*/