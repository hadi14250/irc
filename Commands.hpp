#pragma once

#include "Server.hpp"
#include "Message.hpp"
#include "Codes.hpp"

//commands should inherit from message
class Commands : public Message
{
public:
	void	CAP();
	void	PASS();
	void	NICK();
	void	USER();
	// void	ERROR();
	// void	OPER();
	// void	PING();
	// void	QUIT();
	// void	MODE();

	// void	JOIN();
	// void	PART();
	// void	TOPIC();
	// void	INVITE();
	// void	KICK();

	// void	MOTD();
	// void	MODE();
	// void	PRIVMSG();
	// void	NOTICE();

	// void	WHOIS();

	// void	KILL();
};