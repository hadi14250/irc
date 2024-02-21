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
# include "Commands.hpp"
# include "sstream"

class Client;

enum Tags
{
	LISTENFD,
	CLIENTFD,
};

class Server{
private:
	std::string				_port;
	int						_listenSockfd;
	int						_pfdsCount;
	int						_readBytes;
	int						_change;
	char					_buf[512];
	struct addrinfo*		_serv;
	struct pollfd*			_pfds;
	
	static std::string		_password;
	static std::string		_servername;

	Server();
	Server(Server const &);
	Server const &	operator=(Server const &);

	// void		checkPassword() const;
	void		checkPort() const;
	void		makeListenSockfd();
	void		addNewPfd(int tag);
	void		copyPfdMapToArray();
	void		deletePfd(int fd);
	void		readMsg(int fd);
	void		sendMsg(int fd);
	static void	signalHandler(int signum);

	void	printPfdsMap();

public:
	static volatile sig_atomic_t 		_run;
	static std::map<int, Client>		_pfdsMap;
	static std::map<std::string, int>	_nickMap;
	
	Server(std::string const & port, std::string const & pswd);
	~Server();

	// static void			printPasswordPolicy();
	void				createServer();
	static void			setSignals();
	static std::string	getPassword();
	static std::string	getServername();

	/********************** TEMPORARY ***********************/
	void	testParse(Commands & msg);
	void	trimTrailingWhitespace(std::string& str);
	
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