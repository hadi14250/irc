#include "Server.hpp"

Server::Server(std::string const & port, std::string const & pswd)
	:	_port(port),
		_password(pswd)
{
	//check for password validity and partial port validity here (we will check port validity again with bind)
	checkPassword();
	checkPort();
}

Server::~Server()
{
	//maybe we have to free things later in here so check for this
}

void	Server::printPasswordPolicy()
{
	std::cout	<< "Password must be 8 - 12 characters in length and may only"
				<< "contain uppercase letters, lowercase letters, numbers, and "
				<< "the following symbols: !, @, $, *"
				<< std::endl;
}

void	Server::checkPassword() const
{
	size_t	len = _password.length();

	if (len < 8 || len > 12)
		throw InvalidPasswordException();
	for (int i = 0; i < len; i++)
	{
		if (!std::isalnum(_password[i]) || _password[i] != '!' || _password[i] != '@' ||
			_password[i] != '$' || _password[i] != '*')
			throw InvalidPasswordException();
	}
}

//this method does not accept a port that is entered as 3000.0 even though 
void	Server::checkPort() const
{
	int	port;

	if (_port.length() > 5)
		throw InvalidPortException();
	std::istringstream ss(_port);
	ss >> port;
	if (ss.fail() || ss.get() != EOF ||
		port < 0 || port > 65535)
		throw InvalidPortException();
}
