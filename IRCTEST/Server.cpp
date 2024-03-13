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
	std::cout << RED << "Server destructor called\n" << RESET;
}

/* 
convert string to int with istringstream and check. 65535 is max port number
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

void	Server::makeListenSockfd()
{
	struct addrinfo	hints;
	int yes = 1;

	std::memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

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

void	Server::deletePfd(int fd)
{
	Client & client = _pfdsMap[fd];
	std::vector<Channel*> clientChanList = client._channels;
	std::string nick = client._nick;

	for (std::vector<Channel*>::iterator chanIt = clientChanList.begin(); chanIt != clientChanList.end(); chanIt++)
	{
		std::map<Client *, bool>::iterator membersIt = (*chanIt)->_members.begin();
		for (; membersIt != (*chanIt)->_members.end(); membersIt++)
			membersIt->first->_messages.push_back(RPL_QUIT(client._identifier));
		(*chanIt)->_members.erase(&client);
		if ((*chanIt)->_members.empty())
			Server::_chanMap.erase((*chanIt)->getChannelName());
	}
	std::map<std::string, int>::iterator it = _nickMap.find(nick);
	if (it != _nickMap.end())
		_nickMap.erase(nick);
	_pfdsMap.erase(fd);
	close(fd);
	std::cout << RED << "Closed connection for client " << nick << " at fd: " << fd << "!" << RESET << std::endl;

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
			deletePfd(fd);
			return ;
		}
		if (readBytes > 0)
			client._fullMsg += buf;
	}
	if (client.chkOverflow())
	{
		client._messages.push_back(ERR_INPUTTOOLONG(client._nick));
		client._fullMsg.clear();
	}
	else if (client._fullMsg.empty() || (!client._fullMsg.empty() && client._fullMsg.at(client._fullMsg.length() - 1) != '\n'))
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

void	Server::createServer()
{
	std::cout << GREENHL << "CREATING SERVER" << RESET << std::endl;
	makeListenSockfd();
	while (_run == 1)
	{
		if (poll(_pfds, _pfdsCount, -1) == -1)
			throw PollException();
		for (int i = 0; i < _pfdsCount; i++)
		{

			if ((_pfds[i].revents & POLLIN) && _pfds[i].fd == _listenSockfd)
			{
				try
				{
					std::cout << YELLOW << "attempting to add new client" << RESET << std::endl;
					addNewPfd(CLIENTFD);
				}
				catch(const std::exception& e)
				{
					std::cerr << e.what() << '\n';
				}
			}
			else if (_pfds[i].revents & POLLIN)
				readMsg(_pfds[i].fd);
			else if (_pfds[i].revents & POLLOUT)
				sendMsg(_pfds[i].fd);
			else if (_pfds[i].revents & POLLHUP)
				deletePfd(_pfds[i].fd);
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
