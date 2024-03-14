#include "Server.hpp"

volatile sig_atomic_t 			Server::_run = 1;
std::map<int, Client>			Server::_pfdsMap;
std::map<std::string, int>		Server::_nickMap;
std::map<std::string, Channel>	Server::_chanMap;
struct pollfd*					Server::_pfds = NULL;
std::string						Server::_password;
int								Server::_pfdsCount = 0;


Server::Server(std::string const & port, std::string const & pswd)
	:	_port(port),
		_listenSockfd(-1),
		_serv(NULL)
{
	_password = pswd;
	checkPort();
	checkPass();
}

Server::~Server()
{
	if (_serv)
		freeaddrinfo(_serv);
	if (_pfds)
		delete [] _pfds;
	for (std::map<std::string, int>::iterator it = _nickMap.begin(); it != _nickMap.end(); it++)
		close(it->second);
	std::cout << "Server destructor called\n";
}

/* 
1. check port length (so we're not parsing huge numbers or strings)
2. convert string to int with istringstream and check. 65535 is max port number
NOTE: this method does not accept a port that is entered as a decimal (ex 3000.0)
*/
void	Server::checkPort() const
{
	int	port;

	if (_port.length() > 5)
		throw InvalidPortException();
	std::istringstream ss(_port);
	ss >> port;
	if (ss.fail() || ss.get() != EOF ||
		port < 1024 || port > 65535) //ports 0 - 1024 are reserved (system ports)
		throw InvalidPortException();
}

void	Server::checkPass() const
{
	if (_password.length() < 1 || _password.length() > 25)
		throw InvalidPasswordException();
	for (size_t i = 0; i < _password.length(); i++)
	{
		if (!std::isprint(static_cast<unsigned char>(_password[i])))
			throw InvalidPasswordException();
	}
}

/* 
1. make a single struct addrinfo (hints) with info about host for server like: ip type, socket type, flags
2. generate linked list of struct addrinfo (_serv) using getaddrinfo
3. socket() - create a socket fd with info from _serv
4. setsockopt() - sometimes the socket isn't cleared from port right away after a program run - this allows our listen socket to reuse port immediately
5. bind() - bind our socket fd to port
6. listen() - make our socket fd a listening socket with a queue of 5 - check this number later
7. make listen socket nonblocking
8. initialize _pfdsMap, _pfdsCount, and _pfds array to hold our listen socket fd (addNewPfd and copyNewPfdMapToArray)
*/
void	Server::makeListenSockfd()
{
	struct addrinfo	hints;
	int yes = 1;

	std::memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM; //using stream sockets
	hints.ai_flags = AI_PASSIVE; //assign address of local host to socket structures

	//we have NULL as first parameter bc we used AI_PASSIVE flag. otherwise use specific ip address
	if (getaddrinfo(NULL, _port.c_str(), &hints, &_serv) != 0) 
		throw GetaddrinfoException();
	if ((_listenSockfd = socket(_serv->ai_family, _serv->ai_socktype, _serv->ai_protocol)) == -1)
		throw SocketException();
	if (setsockopt(_listenSockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1)
		throw SetsockoptException();
	if (bind(_listenSockfd, _serv->ai_addr, _serv->ai_addrlen) == -1)
		throw BindException();
	if (listen(_listenSockfd, 25) == -1)
		throw ListenException();
	addNewPfd(LISTENFD);
}

/*
1. create new client object (listen or client) and set client's fd
2. fcntl - set fd to nonblocking and check for error
3. create new struct pollfd with inputted fd and set events to POLLIN
4. add fd and Client pair to map
5. set _change to 1
*/
void	Server::addNewPfd(int tag)
{
	Client newClient;
	if (tag == LISTENFD)
	{
		newClient._listenSock = true;
		newClient._sockfd = _listenSockfd;
	}
	else if (tag == CLIENTFD)
	{
		newClient._listenSock = false;
		socklen_t addrlen = sizeof(struct sockaddr_in);
		struct sockaddr_in clientInfo;
		std::memset(&clientInfo, 0, addrlen);
		newClient._sockfd = accept(_listenSockfd, (struct sockaddr *)&clientInfo, &addrlen);
		if (newClient._sockfd == -1)
			throw AcceptException();
		newClient._clientInfo = clientInfo;
	}
	if (fcntl(newClient._sockfd, F_SETFL, O_NONBLOCK) == -1)
		throw FcntlException();
	
	struct pollfd newPfd = {}; 
	newPfd.fd = newClient._sockfd;
	newPfd.events = POLLIN | POLLOUT;
	_pfdsMap[newClient._sockfd] = newClient;
	
	//add new pfd here
	int newPfdsCount = _pfdsMap.size();
	struct pollfd * newPfdsArray = new struct pollfd[newPfdsCount]();
	if (_pfds != NULL)
	{
		std::copy(_pfds, _pfds + _pfdsCount, newPfdsArray);
		delete [] _pfds;
		_pfds = NULL;
	}
	newPfdsArray[newPfdsCount - 1] = newPfd;
	_pfds = newPfdsArray;
	_pfdsCount = newPfdsCount;
}

/* 
1. Print error msg
2. close fd
3. erase fd from map
4. erase fd from nickMap
5. set change to 1
//should this throw some more error messages?
*/
void	Server::deletePfd(int fd)
{
	Client & client = _pfdsMap[fd];
	std::vector<Channel*> clientChanList = client._channels;
	std::string nick = client._nick;

	//use ERROR  command to send to client to report a fatal error (aka shutdown)
	//THIS GAVE US UNDEFINED ERRORS!
	// std::string errorMsg = "ERROR :client quit\r\n";
	// if (send(fd, errorMsg.c_str(), errorMsg.length(), 0) == -1)
	// 	std::cout << "Unable to send error quit message to client\n";

	//iterate through clients channel lists and notify everyone client is quitting
	for (std::vector<Channel*>::iterator chanIt = clientChanList.begin(); chanIt != clientChanList.end(); chanIt++)
	{
		std::map<Client *, bool>::iterator membersIt = (*chanIt)->_members.begin();
		for (; membersIt != (*chanIt)->_members.end(); membersIt++)
			membersIt->first->_messages.push_back(RPL_QUIT(client._identifier));
		//erase client from channel's member list
		(*chanIt)->_members.erase(&client);
		if ((*chanIt)->_members.empty())
			Server::_chanMap.erase((*chanIt)->getChannelName());
	}
	//erase from server maps (_nickMap and _pfdsMap); close fd
	std::map<std::string, int>::iterator it = _nickMap.find(nick);
	if (it != _nickMap.end())
		_nickMap.erase(nick);
	_pfdsMap.erase(fd);
	close(fd);
	std::cout << RED << "Closed connection for client " << nick << " at fd: " << fd << "!" << RESET << std::endl;

	//reset _pfdsCount and resize _pfds
	int newPfdsCount = _pfdsMap.size();
	struct pollfd * newPfdsArray = new struct pollfd[_pfdsCount]();
	int j = 0;
	for (int i = 0; i < _pfdsCount; i++)
	{
		if (_pfds[i].fd == fd)
			continue;
		newPfdsArray[j] = _pfds[i];
		j++;
	}
	delete [] _pfds;
	_pfds = newPfdsArray;
	_pfdsCount = newPfdsCount;
}

void	Server::readMsg(int fd)
{
	Client &	client = _pfdsMap[fd];
	int			readBytes = 1;
	char		buf[513];

	while (readBytes > 0)
	{
		std::memset(buf, 0, 513);
		readBytes = recv(fd, buf, 512, 0);
		if (readBytes == 0)
		{
			deletePfd(fd); //if we don't have this we get a weird infinite loop
			return ;
		}
		if (readBytes > 0)
			client._fullMsg += buf;
	}
	if (client.chkOverflow()) //if message is bigger than 512 bytes without the /r/n
	{
		client._messages.push_back(ERR_INPUTTOOLONG(client._nick));
		client._fullMsg.clear();
	}
	else if (client._fullMsg.empty() || (!client._fullMsg.empty() && client._fullMsg.at(client._fullMsg.length() - 1) != '\n')) //if message is less than 512 bytes but does nt have newline
		return;
	else 
	{
		std::vector<std::string>	cmds = splitPlusPlus(client.getFullMsg(), "\r\n");
		client._fullMsg.clear();
		for (vecStrIt it = cmds.begin(); it != cmds.end(); it++) 
		{
			if (!chkArgs(*it, 1))
				continue ;
			else 
			{
				std::cerr << " processing > " << *it << std::endl;
				Commands	parseCmd(fd, getCmd(*it), removeCmd(*it), _pfdsMap[fd]);
			}
		}
	}
}

void	Server::sendMsg(int fd)
{
	Client &client = _pfdsMap[fd];
	std::deque<std::string>::iterator it = client._messages.begin();
	for (; it != client._messages.end(); it++)
	{
		if (send(fd, (*it).c_str(), (*it).length(), 0) == -1)
			std::cerr << "Failed to send msg: " << *it << std::endl;
	}
	client._messages.clear();
}

/* 
1. create listening socket
In while loop
2. set a switch int change - tracks if any fds have been added to array
3. poll all fds - this function will switch revents (in struct) if fd is ready for some kind of action
4. iterate through every fd and check if there is POLLIN/POLLOUT/POLLHUP in revents
	POLLIN - data can be read from fd without blocking
	POLLOUT - data can be written to fd without blocking
	POLLHUP - fd has been closed or disconnected - delete it from our array
5. if a new client has been added, modify our array accordingly
7. free addrinfo struct (and everything else that needs to be freed)
 */

void	Server::createServer()
{
	std::cout << GREENHL << "CREATING SERVER" << RESET << std::endl;
	makeListenSockfd();
	while (_run == 1)
	{
		//-1 means that poll will block indefinitely until it gets something from any file descriptors in _pfds
		if (poll(_pfds, _pfdsCount, -1) == -1)
			throw PollException();
		for (int i = 0; i < _pfdsCount; i++)
		{

			if ((_pfds[i].revents & POLLIN) && _pfds[i].fd == _listenSockfd)
			{
				try
				{
					std::cout << YELLOW << "attempting to add new client" << RESET << std::endl;
					addNewPfd(CLIENTFD); //if any errors, exception is thrown before being added to map
				}
				catch(const std::exception& e)
				{
					std::cerr << e.what() << '\n';
				}
			}
			else if (_pfds[i].revents & POLLIN)
			{
				//fd is ready for reading - USE RCV MSG AND PARSING HERE
				readMsg(_pfds[i].fd);
			}
			else if (_pfds[i].revents & POLLOUT)
			{
				//fd is ready for writing - use SEND HERE
				sendMsg(_pfds[i].fd);
			}
			else if (_pfds[i].revents & POLLHUP)
			{
				std::cout << "POLLHUP deleting fd: " << _pfds[i].fd << "\n";
				deletePfd(_pfds[i].fd);
			}
		}
	}
}

void	Server::signalHandler(int signum)
{
	(void)signum;
	_run = 0;
	std::cout << RED << "\nending program\n" << RESET;
}

/* 
1. SIGINT = ctrl+c
2. SIGTSTP = ctrl+z
3. SIGQUIT = ctrl+\
*/
void	Server::setSignals()
{
	std::signal(SIGINT, signalHandler);
	std::signal(SIGTSTP, signalHandler);
	std::signal(SIGQUIT, signalHandler);
}

std::string	Server::getPassword()
{
	return _password;
}
