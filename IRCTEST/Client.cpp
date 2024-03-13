#include "Client.hpp"

Client::Client()
	:	_nick("*"),
		_isBotFirstCall(true),
		_listenSock(true),
		_authenticated(false),
		_registered(false)
{
}

bool	Client::appendBuffer(char *buf) {
	if (*buf)
		_fullMsg += buf;
	return ((_fullMsg.size() && _fullMsg.at(_fullMsg.size() - 1) == '\n') ? true : false);
}

bool	Client::chkOverflow() {
	return ( (_fullMsg.size() > 512 || ( (_fullMsg.size() == 512) && (_fullMsg.at(_fullMsg.size() - 1) != '\n') ) ? true : false) );
}

std::string	Client::getFullMsg() {
	return _fullMsg;
}
