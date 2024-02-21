#pragma once

//WELCOME
#define RPL_WELCOME(servername, nick, identifier) \
	":" + servername + " 001 " + nick + " :Welcome to the Internet Relay Network " + identifier + "\r\n"
#define RPL_YOURHOST(servername, nick) \
	":" + servername + " 002 " + nick + " :Your host is " + servername + "\r\n"
#define RPL_CREATED(servername, nick) \
	":" + servername + " 003 " + nick + " :This server was created February 29, 2024\r\n"
#define RPL_MYINFO(servername, nick) \
	":" + servername + " 004 " + nick + " " + "\r\n"
#define RPL_ISUPPORT(servername, nick) \
	":" + servername + " 005 " + nick + " " + "\r\n"
#define RPL_MOTD(servername, nick) \
	":" + servername + " 372 " + nick + " :-MOTD MAIN BODY\r\n"
#define RPL_MOTDSTART(servername, nick) \
	":" + servername + " 375 " + nick + " :-Message of the day\r\n"
#define RPL_ENDOFMOTD(servername, nick) \
	":" + servername + " 376 " + nick + " :End of /MOTD command\r\n"
//PASS / USER
//we might need to add command here
#define ERR_NEEDMOREPARAMS(command) \
	"FAIL " + command + " 461 :Need more parameters\r\n"
#define ERR_ALREADYREGISTERED(servername, nick) \
	":" + servername + " 462 " + nick + " :You are already registered\r\n"
#define ERR_PASSWDMISMATCH(servername, nick) \
	":" + servername + " 464 " + nick + " :You are already registered\r\n"

//NICK
#define ERR_NONICKNAMEGIVEN(servername, nick) \
	":" + servername + " 431 " + nick + " :No nickname given\r\n"
#define ERR_ERRONEUSNICKNAME(servername, nick) \
	":" + servername + " 432 " + nick + " :Invalid nickname\r\n"
#define ERR_NICKNAMEINUSE(servername, nick) \
	":" + servername + " 433 * " + nick + " :Nickname already in use\r\n"
#define NICKNAME(identifier, nick) \
	":" + identifier + " NICK :" + nick + "\r\n"