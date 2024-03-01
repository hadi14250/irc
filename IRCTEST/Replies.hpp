
#ifndef REPLIES_HPP
#define REPLIES_HPP

//! hadi -> btw do we really need to pass the servername for eah call since its constant?

#define RPL_WELCOME(nick) \
":" + SERVER_NAME + " 001 " + nick + " :Welcome to the Internet Relay Network " + nick + "\n"
#define RPL_WELCOMEBACK(nick) \
":" + SERVER_NAME + " 001 " + nick + " :Welcome back to the Internet Relay Network " + nick + "\n"
#define RPL_YOURHOST(nick, version) \
":" + SERVER_NAME + " 002 " + nick + " :Your host is " + SERVER_NAME + ", running version " + version + "\n"
#define RPL_CREATED(nick) \
":" + SERVER_NAME + " 003 " + nick + " :This server was created November 5th, 2022\n"
#define RPL_MYINFO(nick, version, usermodes, chanmodes) \
":" + SERVER_NAME + " 004 " + nick + " " + SERVER_NAME + " " + version + " " + usermodes + " " + chanmodes + " :" + SERVER_NAME + " server\n"
#define RPL_ISUPPORT 005
#define RPL_BOUNCE 010
#define RPL_UMODEIS(nick, modes) \
":" + SERVER_NAME + " 221 " + nick + " :+" + modes + "\n";
#define RPL_LUSERCLIENT 251
#define RPL_LUSEROP 252
#define RPL_LUSERUNKNOWN 253
#define RPL_LUSERCHANNELS 254
#define RPL_LUSERME 255
#define RPL_ADMINME 256
#define RPL_ADMINLOC1 257
#define RPL_ADMINLOC2 258
#define RPL_ADMINEMAIL 259
#define RPL_TRYAGAIN 263
#define RPL_LOCALUSERS 265
#define RPL_GLOBALUSERS 266
#define RPL_WHOISCERTFP 276
#define RPL_NONE 300
#define RPL_AWAY 301
#define RPL_USERHOST 302
#define RPL_UNAWAY 305
#define RPL_NOWAWAY 306
#define RPL_WHOREPLY 352
#define RPL_ENDOFWHO 315
#define RPL_WHOISREGNICK 307
#define RPL_WHOISUSER 311
#define RPL_WHOISSERVER 312
#define RPL_WHOISOPERATOR 313
#define RPL_WHOWASUSER 314
#define RPL_WHOISIDLE 317
#define RPL_ENDOFWHOIS 318
#define RPL_WHOISCHANNELS 319
#define RPL_WHOISSPECIAL 320
#define RPL_LISTSTART(nick) \
	":" + SERVER_NAME + " 321 " + nick + " Channel :Users Name\n"
#define RPL_LIST(nick, channel, users, mode, topic) \
	":" + SERVER_NAME + " 322 " + nick + " " + channel + " " + users + " " + mode + " :" + topic + "\n"
#define RPL_LISTEND(nick) \
	":" + SERVER_NAME + " 323 " + nick + " :End of /LIST\n"
#define RPL_CHANNELMODEIS(nick, channel, modes) \
	":" + SERVER_NAME + " 324 " + nick + " " + channel + " :+" + modes + "\n"
#define RPL_CREATIONTIME 329
#define RPL_WHOISACCOUNT 330
#define RPL_NOTOPIC(nick, channel) ":" + SERVER_NAME + " 331 " + nick + " " + channel + " :No topic is set\n"
#define RPL_TOPIC(nick, channel, topic) \
	":" + SERVER_NAME + " 332 " + nick + " " + channel + " :" + topic + "\n"
#define RPL_TOPICWHOTIME 333
#define RPL_WHOISACTUALLY 338
#define RPL_INVITING 341
#define RPL_INVITELIST 346
#define RPL_ENDOFINVITELIST 347
#define RPL_EXCEPTLIST 348
#define RPL_ENDOFEXCEPTLIST 349
#define RPL_VERSION 351
#define RPL_NAMREPLY(nick, symbol, channel, users) \
	":" + SERVER_NAME + " 353 " + nick + " " + symbol + " #" + channel + " :" + users + "\n"
#define RPL_ENDOFNAMES(nick, channel) \
	":" + SERVER_NAME + " 366 " + nick + " " + channel + " :End of /NAMES list\n"
#define RPL_BANLIST 367
#define RPL_ENDOFBANLIST 368
#define RPL_ENDOFWHOWAS 369
#define RPL_INFO 371
#define RPL_ENDOFINFO 374
#define RPL_MOTDSTART 375
#define RPL_MOTD 372
#define RPL_ENDOFMOTD 376
#define RPL_WHOISHOST 378
#define RPL_WHOISMODES 379
#define RPL_YOUREOPER 381
#define RPL_REHASHING 382
#define RPL_TIME 391
#define ERR_UNKNOWNERROR 400
#define ERR_NOSUCHNICK(nick) \
	":" + SERVER_NAME + " 401 " + nick + " :No such nick/channel\n"
#define ERR_NOSUCHSERVER 402
#define ERR_NOSUCHCHANNEL(nick, channel) \
	":" + SERVER_NAME + " 403 " + nick + " " + channel + " :No such channel\n"
#define ERR_CANNOTSENDTOCHAN 404
#define ERR_TOOMANYCHANNELS 405
#define ERR_WASNOSUCHNICK 406
#define ERR_NOORIGIN 409
#define ERR_INVALIDCAPCMD(nick, command) \
":" + SERVER_NAME + " 410 " + nick + ' ' + command + " :Invalid CAP command\n"
#define ERR_INPUTTOOLONG 417
#define ERR_UNKNOWNCOMMAND(nick, command) \
":" + SERVER_NAME + " 421 " + nick + ' ' + command + " :Unknown command\n"
#define ERR_NOMOTD 422
#define ERR_NONICKNAMEGIVEN(nick) \
":" + SERVER_NAME + " 431 " + nick + " :No nickname given\n"
#define ERR_ERRONEUSNICKNAME 432
#define ERR_NICKNAMEINUSE(nick) \
	":" + SERVER_NAME + " 433 * " + nick + " :Nickname is already in use\n"
#define ERR_USERNOTINCHANNEL(nick, channel) \
	":" + SERVER_NAME + " 441 * <" + nick + "> isn't on " + channel + "\n"
#define ERR_NOTONCHANNEL(nick, channel) \
	":" + SERVER_NAME + " 442 * " + nick + " :You're not on " + channel + "\n"
#define ERR_USERONCHANNEL(nick) \
	":" + SERVER_NAME + " 443 * " + nick + " :You're already member of that channel\n"
#define ERR_NOTREGISTERED 451
#define ERR_NEEDMOREPARAMS(nick, command) \
	":" + SERVER_NAME + " 461 " + nick + " " + command + " :Wrong num parameters\n"
#define ERR_ALREADYREGISTERED(nick) \
	":" + SERVER_NAME + " 462 " + nick + " :You are Already Registered\n"
#define ERR_PASSWDMISMATCH(nick) \
	":" + SERVER_NAME + " 464 " + nick + " :Password incorrect\n"
#define ERR_YOUREBANNEDCREEP(nick) \
	":" + SERVER_NAME + " 465 " + nick + " :You are banned from this server\n"
#define ERR_CHANNELISFULL(nick, channel) \
	":" + SERVER_NAME + " 471 " + nick + " " + channel + " :Cannot join channel , channel is full(+l)\n"
#define ERR_UNKNOWNMODE(nick, mode, channel) \
	":" + SERVER_NAME + " 472 " + nick + " " + mode + " :is unknown mode char to me for that " + channel + "\n"
#define ERR_INVITEONLYCHAN 473
#define ERR_BANNEDFROMCHAN 474
#define ERR_BADCHANNELKEY(nick, channel) \
	":" + SERVER_NAME + " 475 " + nick + " " + channel + " :Bad channel key\n"
#define ERR_BADCHANMASK(nick) \
	":" + SERVER_NAME + " 476 * " + nick + " :Bad Channel Mask\n"
#define ERR_NOPRIVILEGES ":" + SERVER_NAME + " 481 " + nick + " :You need operator Privilege\n"
#define ERR_CHANOPRIVSNEEDED(nick) ":" + SERVER_NAME + " 482 " + nick + " :You need operator Privilege\n"
#define ERR_CANTKILLSERVER 483
#define ERR_NOOPERHOST 491
#define ERR_UMODEUNKNOWNFLAG(nick, modechar) ":" + SERVER_NAME + " 501 " + nick + " " + mode + " :is unknown mode\n"
#define ERR_USERSDONTMATCH(nick) ":" + SERVER_NAME + " 502 " + nick + " :Cant change mode for other users\n"
#define ERR_HELPNOTFOUND 524
#define ERR_INVALIDKEY 525
#define RPL_STARTTLS 670
#define RPL_WHOISSECURE 671
#define ERR_STARTTLS 691
#define ERR_INVALIDMODEPARAM 696
#define RPL_HELPSTART 704
#define RPL_HELPTXT 705
#define RPL_ENDOFHELP 706
#define ERR_NOPRIVS 723
#define RPL_LOGGEDIN 900
#define RPL_LOGGEDOUT 901
#define ERR_NICKLOCKED 902
#define RPL_SASLSUCCESS 903
#define ERR_SASLFAIL 904
#define ERR_SASLTOOLONG 905
#define ERR_SASLABORTED 906
#define ERR_SASLALREADY 907
#define RPL_SASLMECHS 908

#endif