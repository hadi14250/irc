#pragma once

# include "Server.hpp"
# include "Codes.hpp"
# include "Client.hpp"
# include <vector>
# include <map>

class Client;

class Commands
{
public:
	int							_senderFd;
	std::string 				_command;
	std::vector<std::string>	_param;
	Client&						_sender;
	std::vector<Client *>		_receiver;
	
	Commands(int fd, Client & sender);
	void	printMsg();

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
	void	PONG();
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