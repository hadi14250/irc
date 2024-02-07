#include "Server.hpp"

Server::Server(std::string const & port, std::string const & pswd)
	:	_port(port),
		_password(pswd),
		_listenSocket(-1)
{
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

//this method does not accept a port that is entered as a decimal (ex 3000.0)
void	Server::checkPort() const
{
	int	port;

	if (_port.length() > 5)
		throw InvalidPortException();
	std::istringstream ss(_port);
	ss >> port;
	if (ss.fail() || ss.get() != EOF ||
		port < 0 || port > 65535) //can we use ports 0 - 1024?
		throw InvalidPortException();
}

void	Server::createServer()
{
	struct addrinfo	hints;
	struct addrinfo	*serv;

	memset(&hints, 0, sizeof(hints));
	hints.ai_familY = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	if (getaddrinfo(NULL, _port, &hints, &serv) != 0)
		throw Error();
	if (_listenSocket = socket(serv->ai_family, serv->ai_socktype, serv->ai_protocol) == -1)
		throw Error();
	if (bind(_listenSocket, serv->ai_addr, serv->ai_addrlen) == -1)
		throw Error();
	if (listen(_listenSocket, 5) == -1)
		throw Error();
	
	struct sockaddr_storage clientInfo;
	int	clientSocket;
	socklen_t addr_size = sizeof(struct sockaddr_storage);
	clientSocket = accept(_listenSocket, (struct sockaddr *)&clientInfo, &addr_size);
	if (clientSocket == -1)
		throw Error();
	

	//adding new client
	/* 
	make a new struct sockaddr_in
	socklen_t addr_size
	create new fd with accept
	add to list of clients
	add fd to monitor in poll
	*/
}

