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
	void	printChan();
	void	UNKNOWN();
	// void	ERROR();
	// void	OPER();
	// void	QUIT();

	void	JOIN();
	void	NAMES();
	// void	PART();
	void	TOPIC();
	void	INVITE();
	void	KICK();

	void	MODE();
	// void	MsgChannel();
	void	MsgClient(std::string recipient, std::string text);
	void	PRIVMSG();
	// void	NOTICE();

	void	WHOIS();

	// void	KILL();
};

typedef void (Commands::*cmdPtr)(void);

/* 
:SirVersalot 001 abaz :Welcome to the realm of SirVersalot! Thy presence is celebrated, and the gates of this server stand open to thee. May thy stay be filled with camaraderie and fruitful discourse
:SirVersalot 002 abaz :Thine host be SirVersalot of ircserv, running version 0.0.69
:SirVersalot 003 abaz :This server hath been forged on the first of January in the year 2024 of our age
:SirVersalot 004 abaz :SirVersalot v0.0.69 io iotkl
:SirVersalot 005 abaz CHANLIMIT=#:5 CHANMODES=,k,l,it CHANNELLEN=20 CHANTYPES=# HOSTLEN=20 KICKLEN=69 MAXLIST=0 MODES=2 NICKLEN=30 PREFIX=(o)@ SILENCE USERLEN=10 :are supported by this server
:SirVersalot 005 abaz TOPICLEN=69 TARGMAX=PRIVMSG:3,JOIN:3,KICK:3,NAMES:3 :are supported by this server
:SirVersalot 422 abaz :MOTD File is missing

: FT_IRC 001 abc :Welcome to the Internet Relay Network abc!~r@r
: FT_IRC 002 abc :Your host is FT_IRC
: FT_IRC 003 abc :This server was created February 29, 2024
: FT_IRC 004 abc :FT_IRC v1 io iotkl
: FT_IRC 005 abc CHANMODES=,k,l,it :are supported by this server
: FT_IRC 422 abc :MOTD File is missing

 Irssi v1.4.5 - https://irssi.org                                                                                   
12:51 -!-  ___           _
12:51 -!- |_ _|_ _ _____(_)
12:51 -!-  | || '_(_-<_-< |
12:51 -!- |___|_| /__/__/_|
12:51 -!- Irssi v1.4.5 - https://irssi.org
12:51 -!- Irssi: The following settings were initialized
12:51                        real_name Unknown
12:51                        user_name user
12:51                             nick user
12:52 -!- Irssi: Looking up host.docker.internal
12:52 -!- Irssi: Connecting to host.docker.internal [192.168.65.2] port 6667
12:52 -!- Irssi: Unable to connect server host.docker.internal port 6667 [Connection refused]
12:52 -!- Irssi: Looking up host.docker.internal
12:52 -!- Irssi: Connecting to host.docker.internal [192.168.65.2] port 2222
12:52 Waiting for CAP LS response...
12:52 -!- Irssi: Connection to host.docker.internal established
12:52 -!- Capabilities supported: 
12:52 -!- FT_IRC 451 *  :You have not registered!
12:52 -!- FT_IRC 001 user :Welcome to the Internet Relay Network user!~user@user
12:52 -!- FT_IRC 002 user :Your host is FT_IRC
12:52 -!- FT_IRC 003 user :This server was created February 29, 2024
12:52 -!- FT_IRC 004 user :FT_IRC v1 io iotkl
12:52 -!- FT_IRC 005 user CHANMODES=,k,l,it :are supported by this server
12:52 -!- FT_IRC 422 user :MOTD File is missing
12:52 -!- Irssi: Not connected to server

 Irssi v1.4.5 - https://irssi.org                                                                                   
12:53 -!-  ___           _
12:53 -!- |_ _|_ _ _____(_)
12:53 -!-  | || '_(_-<_-< |
12:53 -!- |___|_| /__/__/_|
12:53 -!- Irssi v1.4.5 - https://irssi.org
12:53 -!- Irssi: The following settings were initialized
12:53                        real_name Unknown
12:53                        user_name user
12:53                             nick user
12:53 -!- Irssi: Looking up host.internal.docker
12:53 -!- Irssi: Unable to connect server host.internal.docker port 2222 [Name or service not known]
12:53 -!- Irssi: Looking up host.docker.internal
12:53 -!- Irssi: Connecting to host.docker.internal [192.168.65.2] port 2222
12:53 Waiting for CAP LS response...
12:53 -!- Irssi: Connection to host.docker.internal established
12:53 -!- Capabilities supported: 
12:53 -!- Welcome to the realm of SirVersalot! Thy presence is celebrated, and the gates of this server stand open 
          to thee. May thy stay be filled with camaraderie and fruitful discourse
12:53 -!- Thine host be SirVersalot of ircserv, running version 0.0.69
12:53 -!- This server hath been forged on the first of January in the year 2024 of our age
12:53 -!- SirVersalot v0.0.69 io iotkl
12:53 -!- CHANLIMIT=#:5 CHANMODES=,k,l,it CHANNELLEN=20 CHANTYPES=# HOSTLEN=20 KICKLEN=69 MAXLIST=0 MODES=2 
          NICKLEN=30 PREFIX=(o)@ SILENCE USERLEN=10 are supported by this server
12:53 -!- TOPICLEN=69 TARGMAX=PRIVMSG:3,JOIN:3,KICK:3,NAMES:3 are supported by this server
12:53 -!- MOTD File is missing



Client No: 1 added to the network!
parsing: <CAP LS 302>
send <CAP * LS :
>
parsing: <JOIN :>
send <:SirVersalot 451 * :You have not registered
>
parsing: <CAP END>
parsing: <PASS 222>
parsing: <NICK user>
parsing: <USER user user host.docker.internal :Unknown>
send <:SirVersalot 001 user :Welcome to the realm of SirVersalot! Thy presence is celebrated, and the gates of this server stand open to thee. May thy stay be filled with camaraderie and fruitful discourse
>
send <:SirVersalot 002 user :Thine host be SirVersalot of ircserv, running version 0.0.69
>
send <:SirVersalot 003 user :This server hath been forged on the first of January in the year 2024 of our age
>
send <:SirVersalot 004 user :SirVersalot v0.0.69 io iotkl
>
send <:SirVersalot 005 user CHANLIMIT=#:5 CHANMODES=,k,l,it CHANNELLEN=20 CHANTYPES=# HOSTLEN=20 KICKLEN=69 MAXLIST=0 MODES=2 NICKLEN=30 PREFIX=(o)@ SILENCE USERLEN=10 :are supported by this server
>
send <:SirVersalot 005 user TOPICLEN=69 TARGMAX=PRIVMSG:3,JOIN:3,KICK:3,NAMES:3 :are supported by this server
>
send <:SirVersalot 422 user :MOTD File is missing
>
parsing: <MODE user +i>
parsing: <PING SirVersalot>
 */