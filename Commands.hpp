#pragma once

# include "Server.hpp"
# include "Codes.hpp"
# include "Client.hpp"
# include "Channel.hpp"
# include <vector>


class Client;
class Channel;

class Commands {
	int							_senderFd;
	std::string					_command;
	std::string					_param;
	Client&						_sender;
	// std::vector<std::string>	_receiver;
	// handle eggor!

public:
	Commands(int fd, std::string command, std::string param, Client& sender);

	void	WelcomeMsg();
	void	MOTD();
	void	completeRegistration(std::string nick);

	void	CAP();
	void	PASS();
	void	NICK();
	void	USER();
	void	UNKNOWN();
	// void	ERROR();
	// void	OPER();
	// void	PONG();
	// void	QUIT();
	// void	MODE();

	void	JOIN();
	// void	PART();
	// void	TOPIC();
	// void	INVITE();
	// void	KICK();

	// void	MODE();
	// void	MsgChannel();
	void	MsgClient(std::string recipient, std::string text);
	void	PRIVMSG();
	// void	NOTICE();

	// void	WHOIS();

	// void	KILL();
};

typedef void (Commands::*cmdPtr)(void);
