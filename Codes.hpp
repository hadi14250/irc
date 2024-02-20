#pragma once

//WELCOME
#define RPL_WELCOME(servername, nick, identifier) \
":" + servername + " 001 " + nick + " :Welcome to the Internet Relay Network " + identifier + "\r\n"

//PASS / USER
//we might need to add command here
#define ERR_NEEDMOREPARAMS (servername, nick) \
":" + servername + " 461 " + nick + " :Need more parameters" + "\r\n"
#define ERR_ALREADYREGISTERED (servername, nick) \
":" + servername + " 462 " + nick + " :You are already registered" + "\r\n"
#define ERR_PASSWDMISMATCH (servername, nick) \
":" + servername + " 464 " + nick + " :You are already registered" + "\r\n"

//NICK
#define ERR_NONICKNAMEGIVEN 431
#define ERR_ERRONEUSNICKNAME 432
#define ERR_NICKNAMEINUSE 433 	