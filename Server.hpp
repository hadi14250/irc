#pragma once

# include <vector>
# include <poll.h>
# include <string>
# include <iostream>
# include <sstream>

//should we make this a static class?
class Server{
private:
	std::string					_port;
	std::string					_password;
	std::vector<struct pollfd>	_fds;

	Server();
	Server(Server const &);
	Server const &	operator=(Server const &);

	void	checkPassword() const;
	void	checkPort() const;
public:
	Server(std::string const & port, std::string const & pswd);
	~Server();

	static void	printPasswordPolicy();

	class InvalidPasswordException : public std::exception{
		const char *what() const throw(){return "Invalid password";}
	};
	class InvalidPortException : public std::exception{
		const char *what() const throw(){return "Invalid port";}
	};
};

/*
Structure:
1. Get user input port and password
2. Check if port is valid (how do we do this?)
3. Enact a password policy and check if password follows rules

*/