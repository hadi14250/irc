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

public:
	Commands(int fd, std::string command, std::string param, Client& sender);

	void	postRegistrationCmds();
	bool	registrationCmds();
	void	WelcomeMsg();
	void	MOTD();
	void	completeRegistration(std::string nick);

	void	CAP();
	void	PASS();
	void	NICK();
	void	USER();
	void	PING();
	void	UNKNOWN();
	// void	ERROR();
	// void	OPER();
	// void	QUIT();

	void	JOIN();
	void	NAMES();
	void	PART();
	void	TOPIC();
	void	INVITE();
	void	KICK();

	void	MODE();
	void	MsgClient(std::string recipient, std::string text);
	void	PRIVMSG();
	// void	NOTICE();// not imp
	// void	WHOIS();// not imp
	// void	KILL();// not imp
};

typedef void (Commands::*cmdPtr)(void);