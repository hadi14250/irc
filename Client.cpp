#include "Client.hpp"

Client::Client()
	:	_mode('\0'),
		_listenSock(true),
		_authenticated(false),
		_registered(false)
{
}

Client::Client(int listenSockfd)
	:	_mode('\0'),
		_listenSock(false),
		_authenticated(false),
		_registered(false)
{
	socklen_t addrlen = sizeof(struct sockaddr_in);
	_sockfd = accept(listenSockfd, (struct sockaddr *)&_clientInfo, &addrlen);
	if (_sockfd == -1)
		throw InvalidClientConnectionException(); //maybe we'll change this error later or check errno
	// const char *msg = "Hello Irrsi!\n";
	// if (send(_sockfd, msg, std::strlen(msg), 0) == -1) 
    //     std::cerr << "Error sending message to client\n";
}

void Client::printClient()
{
	std::cout << "*****PRINTING CLIENT INFO*****\n";
	std::cout	<< "_sockfd: " << _sockfd << "\n"
				<< "_nick: " << _nick << "\n"
				<< "_user: " << _user << "\n"
				<< "_realName: " << _realName << "\n"
				<< "_mode: " << _mode << "\n"
				<< "_listenSock: " << _listenSock << "\n"
				<< "_authenticated: " << _authenticated << "\n"
				<< "_registered: " << _registered << "\n"
				<< "FD STRUCT INFO:" << "\n"
				<< "fd: " << _pfd.fd << "\n";
	if (_pfd.events == POLLIN)
		std::cout << "events: POLLIN\n";
	else if (_pfd.events == POLLOUT)
		std::cout << "events: POLLOUT\n";
	else
		std::cout << "events: garbage value\n";
	if (_pfd.revents == POLLIN)
		std::cout << "revents: POLLIN\n";
	else if (_pfd.revents == POLLOUT)
		std::cout << "revents: POLLOUT\n";
	else
		std::cout << "revents: garbage value\n";
	std::cout 	<< "SOCKADDR_IN CLIENT INFO:\n"
				<< "sin_port: " << _clientInfo.sin_port << "\n";
}



