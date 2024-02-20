#pragma once

//WELCOME
#define RPL_WELCOME(servername, nick, identifier) \
	":" + servername + " 001 " + nick + " :Welcome to the Internet Relay Network " + identifier + "\r\n"

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