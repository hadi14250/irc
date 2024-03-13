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
# include <cctype>
# include "Client.hpp"
# include "Commands.hpp"
# include "Channel.hpp"
# include "Utils.hpp"
# include <algorithm>

typedef std::vector<std::string>::iterator			vecStrIt;
typedef std::map<std::string, Channel>::iterator	chnMapIt;

class Client;
class Channel;

enum Tags
{
	LISTENFD,
	CLIENTFD,
};

class Server {
private:
	std::string				_port;
	int						_listenSockfd;
	struct addrinfo*		_serv;
	static std::string		_password;

	Server();
	Server(Server const &);
	Server const &	operator=(Server const &);

	void		checkPort() const;
	void		checkPass() const;
	void		makeListenSockfd();
	void		addNewPfd(int tag);
	void		copyPfdMapToArray();
	void		readMsg(int fd);
	void		sendMsg(int fd);
	static void	signalHandler(int signum);
public:	
	static volatile sig_atomic_t 			_run;
	static std::map<int, Client>			_pfdsMap;
	static std::map<std::string, int>		_nickMap;
	static std::map<std::string, Channel>	_chanMap;
	static struct pollfd*					_pfds;
	static int								_pfdsCount;
	
	Server(std::string const & port, std::string const & pswd);
	~Server();

	void				createServer();
	static void			setSignals();
	static std::string	getPassword();
	static void			deletePfd(int fd);
	
	/********************** EXCEPTIONS **********************/

	class InvalidPasswordException : public std::exception{
		const char *what() const throw(){return "Invalid password. Rules: 1-25 char(s) long, printable chars only";}
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
