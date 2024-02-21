#pragma once

#include "Server.hpp"
#include "Message.hpp"
#include "Codes.hpp"
#include "Client.hpp"

//commands should inherit from message
class Commands : public Message
{
public:
	void	WelcomeMsg();
	void	MOTD();
	bool	invalidNick();
	void	completeRegistration();

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

	// void	MODE();
	// void	PRIVMSG();
	// void	NOTICE();

	// void	WHOIS();

	// void	KILL();
};