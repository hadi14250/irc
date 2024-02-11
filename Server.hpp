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

	void	checkPassword() const;
	void	checkPort() const;
	void	makeListenSockfd();
	void	addNewPfd(int tag);
	void	copyPfdMapToArray();
	void	deletePfd(int pfd, int err);

	void	printPfdsMap();

public:

	Server(std::string const & port, std::string const & pswd);
	~Server();

	static void	printPasswordPolicy();
	void	createServer();

	
	/********************** EXCEPTIONS **********************/

	class InvalidPasswordException : public std::exception{
		const char *what() const throw(){return "Invalid password";}
	};
	class InvalidPortException : public std::exception{
		const char *what() const throw(){return "Invalid port";}
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