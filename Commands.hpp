#pragma once

#include "Server.hpp"

//commands should inherit from message
class Commands : public Message
{
private:
public:
	void	CAP();
	void	PASS();
	void	NICK();
	void	USER();
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