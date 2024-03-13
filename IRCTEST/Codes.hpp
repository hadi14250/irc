#pragma once

// COLORS
#define RESET "\033[0m"
#define RED "\033[31m"
#define GREEN "\033[32m"
#define YELLOW "\033[33m"
#define GREENHL "\033[1;37;42m"

//!													LIMITS											//
#define	NICK_MAX 15
#define	USER_LEN 10
#define	CHAN_LEN 20
#define	KICK_LEN 50
#define	TOPC_LEN 50


//!													MESSAGES											//

#define NICKNAME(identifier, nick) ":" + identifier + " NICK :" + nick + "\r\n"
#define INVITE_MSG(identifier, recipient, channelName) \
	":" + identifier + " INVITE " + recipient + ' ' + channelName + "\r\n"
#define JOIN_MSG(identifier, channelName) \
	":" + identifier + " JOIN " + channelName + "\r\n"
#define PART_MSG(identifier, channelName) \
	":" + identifier + " PART " + channelName + "\r\n"
#define KICK_MSG(identifier, channelName, recipient, msg) \
	":" + identifier + " KICK " + channelName + ' ' + recipient + " :" + msg + "\r\n"
#define PRIV_MSG(identifier, recipient, msg) \
	":" + identifier + " PRIVMSG " + recipient + " :" + msg + "\r\n"
#define PONG(msg) "PONG :" + msg + "\r\n"



//!											REPLY CODES 0 -> 399									//

#define RPL_WELCOME(nick, identifier) \
	":FT_IRC 001 " + nick + " :Welcome to the Internet Relay Network " + identifier + "\r\n"
#define RPL_YOURHOST(nick) \
	":FT_IRC 002 " + nick + " :Your host is FT_IRC\r\n"
#define RPL_CREATED(nick) \
	":FT_IRC 003 " + nick + " :This server was created February 29, 2024\r\n"
#define RPL_MYINFO(nick) \
	":FT_IRC 004 " + nick + " :FT_IRC v1 io iotkl\r\n"
#define RPL_ISUPPORT(nick) \
	":FT_IRC 005 " + nick + " CHANMODES=,k,l,it CHANTYPES=#& CHANNELLEN=20 KICKLEN=50 NICKLEN=15 USERLEN=10 TOPICLEN=50 :are supported by this server\r\n"

#define RPL_WHOISUSER(nick, reqnick, username, host, realname) \
	":FT_IRC 311 " + nick + " " + reqnick + " " + username + " " + host + " * " + realname + "\r\n"
#define RPL_WHOISSERVER(nick, reqnick, server) \
	":FT_IRC 312 " + nick + " " + reqnick + " " + server + " :FT_IRC\r\n"
#define RPL_ENDOFWHOIS(nick) \
	":FT_IRC 318 " + nick + " :End of /WHOIS list\r\n"

#define RPL_CHANNELMODEIS(nick, channelName, modeStr, modeArgs) \
	":FT_IRC 324 " + nick + ' ' + channelName + ' ' + modeStr + ' ' + modeArgs + "\r\n"

#define RPL_NOTOPIC(nick, channelName) \
	":FT_IRC 331 " + nick + ' ' + channelName + " :No topic is set\r\n"
#define RPL_TOPIC(nick, channelName, topic) \
	":FT_IRC 332 " + nick + ' ' + channelName + " :" + topic + "\r\n"
#define RPL_TOPICWHOTIME(nick, channelName, author, topiCreat) \
	":FT_IRC 333 " + nick + ' ' + channelName + ' ' + author + " :" + topiCreat + "\r\n"

#define RPL_INVITING(nick, recipient, channelName) \
	":FT_IRC 341 " + nick + ' ' + recipient + ' ' + channelName + "\r\n"

#define RPL_NAMREPLY(nick, channelName, args) \
	":FT_IRC 353 " + nick + " = " + channelName + " :" + args + "\r\n"

#define RPL_ENDOFNAMES(nick, channelName) \
	":FT_IRC 366 " + nick + " " + channelName + " :End of /NAMES list\r\n"

#define RPL_MOTD(nick) \
	":FT_IRC 372 " + nick + " :-MOTD MAIN BODY\r\n"
#define RPL_MOTDSTART(nick) \
	":FT_IRC 375 " + nick + " :-Message of the day\r\n"
#define RPL_ENDOFMOTD(nick) \
	":FT_IRC 376 " + nick + " :End of /MOTD command\r\n"


//!											ERROR CODES 400 -> 696									//

#define ERR_UNKNOWNERROR(nick, cmd, subCmd, info) \
	":FT_IRC 400 " + nick + ' ' + cmd + ' ' + subCmd + " :" + info + "\r\n"
#define ERR_NOSUCHNICK(nick, rcp_nick) \
	":FT_IRC 401 " + nick + ' ' + rcp_nick + " :No such nick\r\n"
#define ERR_NOSUCHCHANNEL(nick, channelName) \
	":FT_IRC 403 " + nick + ' ' + channelName + " :No such channel\r\n"

#define ERR_INVALIDCAPCMD(nick, command) \
	":FT_IRC 410 " + nick + ' ' + command + " :Invalid CAP command\r\n"
#define ERR_NORECIPIENT(nick) \
	":FT_IRC 411 " + nick + " :No recipient given\r\n"
#define ERR_NOTEXTTOSEND(nick) \
	":FT_IRC 412 " + nick + " :No text to send\r\n"
#define ERR_INPUTTOOLONG(nick) \
	":FT_IRC 417 " + nick + " :Input line was too long\r\n"

#define ERR_UNKNOWNCOMMAND(nick, command) \
	":FT_IRC 421 " + nick + ' ' + command + " :Unknown command\r\n"
#define ERR_NOMOTD(nick) \
	":FT_IRC 422 " + nick + " :MOTD File is missing\r\n"

#define ERR_NONICKNAMEGIVEN(nick) \
	":FT_IRC 431 " + nick + " :No nickname given\r\n"
#define ERR_ERRONEUSNICKNAME(nick) \
	":FT_IRC 432 " + nick + " :Invalid nickname\r\n"
#define ERR_NICKNAMEINUSE(nick) \
	":FT_IRC 433 * " + nick + " :Nickname already in use\r\n"

#define ERR_USERNOTINCHANNEL(nick, recip, channelName) \
	":FT_IRC 441 " + nick + ' ' + recip + ' ' + channelName + " :Key is not well-formed\r\n"
#define ERR_NOTONCHANNEL(nick, channelName) \
	":FT_IRC 442 " + nick + ' ' + channelName + " :You're not on that channel\r\n"
#define ERR_USERONCHANNEL(nick, recipient, channelName) \
	":FT_IRC 443 " + nick + ' ' + recipient + ' ' + channelName + " :is already on channel\r\n"

#define ERR_NOTREGISTERED(nick, msg) \
	":FT_IRC 451 " + nick + ' ' + " :" + msg + "\r\n"

#define ERR_NEEDMOREPARAMS(nick, command) \
	":FT_IRC 461 " + nick + ' ' + command + " :Need more parameters\r\n"
#define ERR_ALREADYREGISTERED(nick) \
	":FT_IRC 462 " + nick + " :You are already registered\r\n"
#define ERR_PASSWDMISMATCH(nick) \
	":FT_IRC 464 " + nick + " :Password incorrect\r\n"
#define ERR_KEYSET(nick, channelName) \
	":FT_IRC 467 " + nick + ' ' + channelName + " :Channel key already set\r\n"

#define ERR_CHANNELISFULL(nick, channelName) \
	":FT_IRC 471 " + nick + ' ' + channelName + " :Cannot join channel (+l), channel is full!\r\n"
#define ERR_UMODEUNKNOWNFLAG(nick) \
	":FT_IRC 472 " + nick + " :Unknown MODE flag\r\n"
#define ERR_INVITEONLYCHAN(nick, channelName) \
	":FT_IRC 473 " + nick + ' ' + channelName + " :Cannot join channel (+i), invite only channel!\r\n"
#define ERR_BADCHANNELKEY(nick, channelName) \
	":FT_IRC 475 " + nick + ' ' + channelName + " :Cannot join channel (+k), please provide channel's key!\r\n"
#define ERR_BADCHANMASK(nick, channelName, msg) \
	":FT_IRC 476 " + nick + ' ' + channelName + " :" + msg + "\r\n"
#define ERR_BADCHANNAME(nick, channame) \
	":FT_IRC 479 " + nick + " " + channame + " :Illegal channel name\r\n"
#define ERR_CHANOPRIVSNEEDED(nick, channelName) \
	":FT_IRC 482 " + nick + ' ' + channelName + " :You're not channel operator\r\n"

#define ERR_UNKNOWNMODE(nick, mode) \
	":FT_IRC 501 " + nick + ' ' + mode + " :Unknown mode character!\r\n"

#define ERR_INVALIDKEY(nick, channelName) \
	":FT_IRC 525 " + nick + ' ' + channelName + " :Key is not well-formed\r\n"

#define ERR_INVALIDMODEPARAM(nick, recip, mode_char, param, description) \
	":FT_IRC 696 " + nick + ' ' + recip + ' ' + mode_char + ' ' + param + " :" + description + "\r\n"

#define RPL_QUIT(identifier) \
	":" + identifier + " QUIT :Client quit\r\n"



