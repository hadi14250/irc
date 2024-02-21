#include "Commands.hpp"

//server "PONG" server nick
void	Commands::PONG(){
	_sender._messages.push_back(Server::getServername()
        + " PONG " + Server::getServername() + " :" + _sender._nick + "\r\n");
}

void    Commands::OPER(){
    _
}