#include "Client.hpp"

Client::Client()
	:	_nick("*"),// when returning either an error or reply, if nick is empty u use '*' instead so it made sence to me to set it as default! feel free to remove this comment now that u know y I did this!
		_listenSock(false),
		_authenticated(false),
		_registered(false),
		_isOper(false);
{
}

Client & Client::operator=(Client const & src)
{
	(void) src;
	return *this;
}

void	Client::printPendingMsgs()
{
	std::cout << "Pending messages to be sent out for client " << _nick << ":\n";
	for (std::deque<std::string>::iterator it = _messages.begin(); it != _messages.end(); it++)
		std::cout << *it << "\n";
}

void Client::printClient()
{
	std::cout << "*****PRINTING CLIENT INFO*****\n";
	std::cout	<< "_sockfd: " << _sockfd << "\n"
				<< "_nick: " << _nick << "\n"
				<< "_username: " << _username << "\n"
				<< "_hostname: " << _hostname << "\n"
				<< "_server: " << _server << "\n"
				<< "_realname: " << _realname << "\n"
				<< "_identifier: " << _identifier << "\n"
				<< "_listenSock: " << _listenSock << "\n"
				<< "_authenticated: " << _authenticated << "\n"
				<< "_registered: " << _registered << "\n"
				<< "_isOper: " << _isOper << "\n"
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
}



