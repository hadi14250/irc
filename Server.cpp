#include "Server.hpp"

Server::Server(std::string const & port, std::string const & pswd)
	:	_port(port),
		_password(pswd),
		_listenSockfd(-1),
		_pfdsCount(0),
		_serv(NULL),
		_pfds(NULL)
{
	checkPort();
	checkPassword();
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
2. generate linked list of struct addrinfo (_serv) using getaddrinfo
3. socket() - create a socket fd with info from _serv
4. bind() - bind our socket fd to port
5. listen() - make our socket fd a listening socket with a queue of 5 - check this number later
6. make listen socket nonblocking
7. initialize _pfdsMap, _pfdsCount, and _pfds array to hold our listen socket fd (addNewPfd and copyNewPfdMapToArray)
*/
void	Server::makeListenSockfd()
{
	struct addrinfo	hints;

	std::memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM; //using stream sockets
	hints.ai_flags = AI_PASSIVE; //assign address of local host to socket structures

	//we have NULL as first parameter bc we used AI_PASSIVE flag. otherwise use specific ip address
	//NOTE: we may want to throw custom error msgs so keeping statements separate for now
	if (getaddrinfo(NULL, _port.c_str(), &hints, &_serv) != 0) 
		throw Error();
	if ((_listenSockfd = socket(_serv->ai_family, _serv->ai_socktype, _serv->ai_protocol)) == -1)
		throw Error();
	// fcntl(_listenSockfd, F_SETFL, O_NONBLOCK);
	if (bind(_listenSockfd, _serv->ai_addr, _serv->ai_addrlen) == -1)
		throw Error();
	if (listen(_listenSockfd, 5) == -1)
		throw Error();
	addNewPfd(LISTENFD);
	copyPfdMapToArray();
}

/*
1. create new client object - different object is created if it's a listening fd
2. set fd to nonblocking and check for error
3. create new struct pollfd with inputted fd and set events to POLLIN
4. add fd and Client pair to map
*/
void	Server::addNewPfd(int tag)
{
	Client newClient;
	if (tag == LISTENFD)
	{
		newClient = Client();
		newClient._sockfd = _listenSockfd; //set our fd to listensockfd;
	}
	else if (tag == CLIENTFD)
	{
		std::cout << "making new client object\n";
		newClient = Client(_listenSockfd); //this constructor fills out _sockfd for us
	}
	if (fcntl(newClient._sockfd, F_SETFL, O_NONBLOCK) == -1)
		throw Error();
	
	struct pollfd newPfd = {};
	newPfd.fd = newClient._sockfd;
	newPfd.events = POLLIN;
	newClient._pfd = newPfd;

	_pfdsMap[newClient._sockfd] = newClient;
}

/* 
1. update _pfdsCount to _pfdsMap.length()
2. delete _pfds array if needed and create new array based on Map
*/
void	Server::copyPfdMapToArray()
{
	_pfdsCount = _pfdsMap.size();	

	if (_pfds)
		delete [] _pfds;
	_pfds = new struct pollfd[_pfdsCount]();
	int i = 0;
	for (std::map<int, Client>::iterator it = _pfdsMap.begin(); it != _pfdsMap.end(); it++)
		_pfds[i++] = it->second._pfd;
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

void	Server::createServer()
{
	//create a listening socket
	makeListenSockfd();
	int yes = 1;
	setsockopt(_listenSockfd, SOL_SOCKET,SO_REUSEADDR, &yes, sizeof(int));
	while (true)
	{
		int change = 0;
		//-1 means that poll will block indefinitely until it gets something from any file descriptors in _pfds
		std::cout << "running poll\n";
		int pollCount = poll(_pfds, _pfdsCount, -1);
		std::cout << "polled\n";
		if (pollCount == -1)
			throw Error();
		std::cout << "Pollcount: " << pollCount << "\n";
		for (int i = 0; i < _pfdsCount; i++)
		{
			// if (i == pollCount) //if we've handled all fds that have an event, we don't have to iterate through the rest
			// 	break ;
			//if it's the listen socket and revents is set to POLLIN
			if ((_pfds[i].revents & POLLIN) && _pfds[i].fd == _listenSockfd)
			{
				try
				{
					std::cout << "attempting to add a new client\n";
					addNewPfd(CLIENTFD);
					change = 1;
					std::cout << "finished adding newpfd\n";
				}
				catch(const std::exception& e)
				{
					std::cerr << e.what() << '\n';
				}
			}
			else if (_pfds[i].revents & POLLIN) //if it's a client, read what client has sent into a buffer
			{
				std::cout << "client messaging\n";
				//510 is max len we can send. see notes
				//I think the parsing goes here -> parse into a message object in client?
				char buf[510] = {}; //can we do this?
				int nbytes = recv(_pfds[i].fd, buf, sizeof(buf), 0); //no flags = 0
				std::cout << "buf: " << buf;
				if (nbytes <= 0)
				{
					std::cout << "recv failed\n";
					deletePfd(_pfds[i].fd, nbytes);
					change = 1;
				}
				else
				{
					
					// executeCmd(_pfdsMap[_pfds[i].fd]);
					//execute the command here if it's valid
					std::cout << "buf: " << buf << "\n";
					std::cout << "we execute something here\n";
					// if (send (_pfds[i].fd, buf, nbytes, 0) == -1)
					// 	std::cout << "send unsuccessful\n";
				}
			}
		}
		if (change == 1)
		{
			copyPfdMapToArray();
			printPfdsMap();
		}
		std::cout << "returning to main while loop\n";
	}
	freeaddrinfo(_serv);
}

void	Server::printPfdsMap()
{
	for (std::map<int, Client>::iterator it = _pfdsMap.begin(); it != _pfdsMap.end(); it++)
	{
		it->second.printClient();
	}
}

/* 
1. If user already authenticated, return
2. if password matches, authenticate
3. else print error message and delete the connection
*/
// void Server::Pass(Client& client)
// {
// 	if (client._authenticated == TRUE)
// 	{
// 		std::cout << "Account already authenticated\n"; //ERR_ALREADYREGISTRED
// 		return ;
// 	}
// 	if (client._msg./*param*/ == _password)
// 		client._authenticated = true;
// 	else
// 	{
// 		std::cerr << "Invalid password entered\n";
// 		deletePFD(client._fd, 0); //connection for client closed! can adjust msg later
// 	}
// }

// /*
// 1. if client is not authenticated, send error message and delete connection
// -nick must be unique and can be changed
// -user does not have to be unique and cannot be changed
// -user comes with real name and mode as well
// */
// void Server::User(Client &client)
// {
// 	if (client._authenticated == FALSE)
// 	{
// 		std::cerr << "Account not authenticated\n"; //we need to send this message back to the client
// 		deletePFD(client._fd, 0); //connectin for client closed!
// 	}
	
	
// }

/* 
NOTES:
to initialize things to 0 for new, add () after
note sockaddr_storage can be used since it's flexible with ipv4 or ipv6.
since we're working with ipv4, better to use sockaddr_in -> then we don't have to static cast the struct after
in c++11 and later, we are actually able to get a pointer from the container using data()

IN WHILE LOOP
1. we run poll on our collection of _pfds and check poll_count
	if poll_count == - 1, error
2. now we loop through our _pfds (this is an inner loop)
	a. We check if the it's the listener fd and if its revents is set at POLLIN
		if yes, it means listener is ready to read!
		create new variables ready to take a client
		-struct sockaddr_in (use this for ipv4, use sockaddr_in for ipv6, or sockaddr_storage if you don't know and then typcast to the correct one after)
		-socklen_t addrsize = sizeof(struct sockaddr_in)
		-int newClientSockfd = accept();
		-if newClientSockfd == -1
			throw error
		-if accept was successful, add new fd to _pfds and set to POLLIN
			-fd reports it is ready to send data to socket
		-update count
		-add to our _pfds
		NOTE: to update our _pfds, I think we should use a container so we can always check its size
			if we ever need to expand our array, we can just delete the array and then create a new one from the vector so we don't have to keep resizing it
	b. if it's the client
		int nbytes = recv()
		if nbytes <= 0
			error or connection was closed
			close the fd
			make sure to delete this client from _pfds!
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

TODO:
CHECK FOR EAGAIN OR EWOULDBLOCK
JOIN #channel-name password

3.1 Connection Registration (see rfc 2812)


   The commands described here are used to register a connection with an
   IRC server as a user as well as to correctly disconnect.

   A "PASS" command is not required for a client connection to be
   registered, but it MUST precede the latter of the NICK/USER
   combination (for a user connection) or the SERVICE command (for a
   service connection). The RECOMMENDED order for a client to register
   is as follows:

                           1. Pass message
           2. Nick message                 2. Service message
           3. User message

   Upon success, the client will receive an RPL_WELCOME (for users) or
   RPL_YOURESERVICE (for services) message indicating that the
   connection is now registered and known the to the entire IRC network.
   The reply message MUST contain the full client identifier upon which
   it was registered.


*/

/* 
DRAFTS:

	// hints.ai_family = AF_UNSPEC; //can be ipv4 or 6

	//for science, lets check how many items are in this linked list _serv
	//there will be two if we use AF_UNSPEC - probably an ipv4 and ipv6
	// struct addrinfo *s;
	// s = _serv;
	// int i = 0;
	// for (; s != NULL; s = s->ai_next)
	// 	i++;
	// std::cout << "there are " << i << " structs in _serv\n";
*/
