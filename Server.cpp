#include "Server.hpp"

Server::Server(std::string const & port, std::string const & pswd)
	:	_port(port),
		_password(pswd),
		_pfds(NULL),
		_pfdsCount(0),
		_listenSocketfd(-1)
{
	checkPassword();
	checkPort();
}

Server::~Server()
{
	if (_pfds)
		delete [] _pfds;
}

/* can change pswd policy later */
void	Server::printPasswordPolicy()
{
	std::cout	<< "Password must be 8 - 12 characters in length and may only "
				<< "contain uppercase letters, lowercase letters, numbers, and "
				<< "the following symbols: !, @, $, *"
				<< std::endl;
}

void	Server::checkPassword() const
{
	size_t	len = _password.length();

	if (len < 8 || len > 12)
		throw InvalidPasswordException();
	for (std::string::const_iterator it = _password.begin(); it != _password.end(); it++)
	{
		if (!std::isalnum(*it) && 
			(*it != '!' || *it != '@' || *it != '$' || *it != '*'))
			throw InvalidPasswordException();
	}
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
		port < 0 || port > 65535) //can we use ports 0 - 1024?
		throw InvalidPortException();
}

/* 
1. make a single struct addrinfo (hints) with info about host for server like: ip type, socket type, flags
2. generate linked list of struct addrinfo (serv) using getaddrinfo
3. socket() - create a socket fd with info from serv
4. bind() - bind our socket fd to port
5. listen() - make our socket fd a listening socket with a queue of 5 - check this number later
6. initialize _pfdsMap, _pfdsCount, and _pfds array to hold our listen socket fd (addNewPfd and copyNewPfdMapToArray)
*/
void	Server::makelistenSockfd()
{
	struct addrinfo	hints;
	struct addrinfo	*serv;

	std::memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM; //using stream sockets
	hints.ai_flags = AI_PASSIVE; //assign address of local host to socket structures

	//we have NULL as first parameter bc we used AI_PASSIVE flag. otherwise use specific ip address
	//NOTE: we may want to throw custom error msgs so keeping statements separate for now
	if (getaddrinfo(NULL, _port.c_str(), &hints, &serv) != 0 ||) 
		throw Error();
	if ((_listenSockfd = socket(serv->ai_family, serv->ai_socktype, serv->ai_protocol)) == -1)
		throw Error();
	if (bind(_listenSockfd, serv->ai_addr, serv->ai_addrlen) == -1)
		throw Error();
	if (listen(listenSockfd, 5) == -1)
		throw Error();
	
	addNewPfd(listenSockfd);
	copyPfdMapToArray();
}

/*
1. create new struct pollfd for input parameter int pfd and add to Map
2. create new _pfds array from map
*/

void	Server::addNewPfd(int pfd)
{
	struct pollfd newPfd = {};
	newPfd.fd = listenSockfd;
	newPfd.events = POLLIN;
	_pfdsMap[listenSockfd] = newPfd;
}

/* 
1. update _pfdsCount to _pfdsMap.length()
2. delete _pfds array if needed and create new array based on Map
*/
void	Server::copyPfdMapToArray()
{
	_pfdsCount = _pfdsMap.length();	

	if (_pfds)
		delete [] _pfds;
	_pfds = new struct pollfd[_pfdsCount]();
	int i = 0;
	for (std::map<int, struct pollfd>::iterator it = _pfdsMap.begin(); it != _pfdsMap.end(); it++)
		_pfds[i++] = it->second;
}

/* 
1. Print error msg
2. close fd
3. erase fd from map
*/
void	Server::deletePfd(int pfd, int err)
{
	if (err == 0)
		std::cout << "connection for client closed!\n";
	else if (err < 0)
		std::cerr << "recv error\n";
	close(pfd);
	_pfdsMap.erase(pfd);
}

/* NOTES:
we need to insert fcntl somewhere??
*/

void	Server::createServer()
{
	//create a listening socket
	makelistenSockfd();

	//we may have to move this into while loop but i dont think so
	socklen_t addrsize = sizeof(struct sockaddr_in);

	while (1)
	{
		//-1 means that poll will block indefinitely until it gets something from any file descriptors in pfds
		int change = 0;
		int pollCount = poll(pfds, _pfdsCount, -1);
		if (pollCount == -1)
			throw Error();
		for (int i = 0; i < _pfdsCount; i++)
		{
			if (i == pollCount) //if we've handled all fds that have an event, we don't have to iterate through the rest
				break ;
			//if it's the listen socket and revents is set to POLLIN
			if (pfds[i] == _listenSocketfd && (pfds[i].revents & POLLIN))
			{
				//check for the password in this block too?
				struct sockaddr_in clientInfo;
				int newPfd = accept(_listenSocketfd, (struct sockaddr *)&clientInfo, &addrlen);
				if (newPfd == -1)
					throw Error(); //but maybe we won't want to exit and jsut print an error here instead
					continue; //???
				addNewPfd(newPFd);
				change = 1;
			}
			else if (pfds[i].revents & POLLIN) //if it's a client, read what client has sent into a buffer
			{
				//510 is max len we can send. see notes
				char buf[510] = {}; //can we do this?
				int nbytes = recv(pfds[i].fd, buf, sizeof(buf), 0); //no flags = 0
				if (nbytes <= 0)
				{
					deletePfd(pfds[i].fd);
					change = 1;
				}
				else
				{
					//send the message according to client parcel
				}
			}
			if (change == 1)
				copyPfdMapToArray();
		}
	}
	freeaddrinfo(serv);
}

/* 
NOTES:
to initialize things to 0 for new, add () after
note sockaddr_storage can be used since it's flexible with ipv4 or ipv6.
since we're working with ipv4, better to use sockaddr_in -> then we don't have to static cast the struct after
in c++11 and later, we are actually able to get a pointer from the container using data()

IN WHILE LOOP
1. we run poll on our collection of pfds and check poll_count
	if poll_count == - 1, error
2. now we loop through our pfds (this is an inner loop)
	a. We check if the it's the listener fd and if its revents is set at POLLIN
		if yes, it means listener is ready to read!
		create new variables ready to take a client
		-struct sockaddr_in (use this for ipv4, use sockaddr_in for ipv6, or sockaddr_storage if you don't know and then typcast to the correct one after)
		-socklen_t addrsize = sizeof(struct sockaddr_in)
		-int newClientSockfd = accept();
		-if newClientSockfd == -1
			throw error
		-if accept was successful, add new fd to pfds and set to POLLIN
			-fd reports it is ready to send data to socket
		-update count
		-add to our pfds
		NOTE: to update our pfds, I think we should use a container so we can always check its size
			if we ever need to expand our array, we can just delete the array and then create a new one from the vector so we don't have to keep resizing it
	b. if it's the client
		int nbytes = recv()
		if nbytes <= 0
			error or connection was closed
			close the fd
			make sure to delete this client from pfds!
		otherwise we've got info! hurray!
			send the data accordingly -> idk how we're going to do this yet 
				but i'm starting to understand why we might need a whole client class
3. check for signals
	ctrl+c will close
	ctrl+d - ??
	ctrl+z - ??


IRC messages are always lines of characters terminated with a CR-LF
(Carriage Return - Line Feed) pair, and these messages SHALL NOT
exceed 512 characters in length, counting all characters including
the trailing CR-LF. Thus, there are 510 characters maximum allowed
for the command and its parameters. - from rfc 2813
*/

/* 
DRAFTS:

	// hints.ai_family = AF_UNSPEC; //can be ipv4 or 6

	//for science, lets check how many items are in this linked list serv
	//there will be two if we use AF_UNSPEC - probably an ipv4 and ipv6
	// struct addrinfo *s;
	// s = serv;
	// int i = 0;
	// for (; s != NULL; s = s->ai_next)
	// 	i++;
	// std::cout << "there are " << i << " structs in serv\n";
*/
