#pragma once

# include <map>
# include <poll.h>
# include <string>
# include <iostream>
# include <sstream>
# include <fstream>
# include <sys/socket.h>
# include <sys/types.h>
# include <netdb.h>
# include <unistd.h>
# include <fcntl.h>
# include <cstring>
# include <csignal>
# include "Client.hpp"

class Client;

enum Tags
{
	LISTENFD,
	CLIENTFD
};

class Server{
private:
	std::string				_port;
	std::string				_password;
	int						_listenSockfd;
	int						_pfdsCount;
	struct addrinfo*		_serv;
	struct pollfd*			_pfds;
	std::map<int, Client>	_pfdsMap;


	Server();
	Server(Server const &);
	Server const &	operator=(Server const &);

	void		checkPassword() const;
	void		checkPort() const;
	void		makeListenSockfd();
	void		addNewPfd(int tag);
	void		copyPfdMapToArray();
	void		deletePfd(int pfd);
	static void	signalHandler(int signum);

	void	printPfdsMap();

public:
	static volatile sig_atomic_t _run;
	
	Server(std::string const & port, std::string const & pswd);
	~Server();

	static void	printPasswordPolicy();
	void		createServer();
	static void	setSignals();

	
	/********************** EXCEPTIONS **********************/

	class InvalidPasswordException : public std::exception{
		const char *what() const throw(){return "Invalid password";}
	};
	class InvalidPortException : public std::exception{
		const char *what() const throw(){return "Invalid port";}
	};
	class GetaddrinfoException : public std::exception{
		const char *what() const throw(){return "getaddrinfo failed";}
	};
	class SocketException : public std::exception{
		const char *what() const throw(){return "socket failed";}
	};
	class BindException : public std::exception{
		const char *what() const throw(){return "bind failed";}
	};
	class SetsockoptException : public std::exception{
		const char *what() const throw(){return "setsockopt failed";}
	};
	class ListenException : public std::exception{
		const char *what() const throw(){return "listen failed";}
	};
	class FcntlException : public std::exception{
		const char *what() const throw(){return "fcntl failed";}
	};
	class PollException : public std::exception{
		const char *what() const throw(){return "poll failed";}
	};
	class AcceptException : public std::exception{
		const char *what() const throw(){return "accept failed";}
	};
	class Error : public std::exception{
		const char *what() const throw(){return "generic error msg";}
	};
};

/*
Structure:
1. Get user input port and password
2. Check if port is valid (how do we do this?)
3. Enact a password policy and check if password follows rules

*/