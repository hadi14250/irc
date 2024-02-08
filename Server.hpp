#pragma once

# include <deque>
# include <poll.h>
# include <string>
# include <iostream>
# include <sstream>
# include <sys/socket.h>
# include <sys/types.h>
# include <netdb.h>

//should we make this a static class?
class Server{
private:
	std::string					_port;
	std::string					_password;
	struct pollfd*				_pfds;
	std::deque<struct pollfd>	_pfdsDeq;
	int							_pfdsCount;
	int							_listenSocketfd;

	Server();
	Server(Server const &);
	Server const &	operator=(Server const &);

	void	checkPassword() const;
	void	checkPort() const;
public:

	Server(std::string const & port, std::string const & pswd);
	~Server();

	static void	printPasswordPolicy();
	void	createServer();

	/********************** UTILITIES **********************/
	


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