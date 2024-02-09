#include "Client.hpp"

Client::Client(int listenSockfd)
		_authenticated(false),
		_registered(false)
{
	socklen_t addrlen = sizeof(struct sockaddr_in);
	_sockfd = accept(listenSockfd, (struct sockaddr *)&_clientInfo, &addrlen);
	if (_sockfd == -1)
		throw InvalidClientConnectionException(); //maybe we'll change this error later or check errno
}


