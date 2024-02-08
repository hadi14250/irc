#include "Server.hpp"

Server::Server(std::string const & port, std::string const & pswd)
	:	_port(port),
		_password(pswd),
		_pfds(new struct pollfd[5]()), //we might not need this, and fdsize should be variable
		_pfdsCount(0),
		_listenSocketfd(-1)
{
	checkPassword();
	checkPort();
}

Server::~Server()
{
	delete [] _pfds;
}

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
	//create a listening socket
	int				listenSockfd;
	struct addrinfo	hints;
	struct addrinfo	*serv; //note that serv is a linked list that is created by getaddrinfo

	memset(&hints, 0, sizeof(hints));
	// hints.ai_family = AF_UNSPEC; //can be ipv4 or 6
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM; //using stream sockets
	hints.ai_flags = AI_PASSIVE; //assign address of local host to socket structures

	//fill out serv struct with relevant info; we have NULL as first parameter bc we used AI_PASSIVE flag. otherwise use specific ip address
	if (getaddrinfo(NULL, _port.c_str(), &hints, &serv) != 0) 
		throw Error();
	//create socket
	if ((listenSockfd = socket(serv->ai_family, serv->ai_socktype, serv->ai_protocol)) == -1)
		throw Error();
	//bind socket to port
	if (bind(listenSockfd, serv->ai_addr, serv->ai_addrlen) == -1)
		throw Error();
	//make socket a listening socket and can take in queue of 5
	if (listen(listenSockfd, 5) == -1)
		throw Error();

	//add listener to pfds and set that listener to POLLIN
	// listener reports it is ready to read incoming connections (recv())
	// _pfds[0].fd = listenSockfd;
	// _pfds[0].events = POLLIN;
	// _pfdsCount++; //update the count
	struct pollfd	newFdtoPoll;
	newFdtoPoll.fd = listenSockfd;
	newFdtoPoll.events = POLLIN;
	_pfdsDeq.push_front(newFdtoPoll);



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
		NOTE: to update our pfds, I think we should use a vector so we can always check its size
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
*/

/* 
DRAFTS:

	//for science, lets check how many items are in this linked list serv
	//there will be two if we use AF_UNSPEC - probably an ipv4 and ipv6
	// struct addrinfo *s;
	// s = serv;
	// int i = 0;
	// for (; s != NULL; s = s->ai_next)
	// 	i++;
	// std::cout << "there are " << i << " structs in serv\n";
*/
