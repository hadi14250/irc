#pragma once

#include <map>
#include <iostream>

#include "Server.hpp"
#include "Channel.hpp"
#include "Client.hpp"
#include <algorithm>
#include <vector>

class Client;

typedef std::map<Client*, bool>::iterator chnMemIt;

class Channel {
	private:
	std::string					_name; //channel name just in case!
	std::string					_topic;
	std::string					_topicAuthor;
	std::string					_topiCreation;// time of topic creation!
	std::string					_pass;//password if channel is password protected!
	unsigned int				_curMemAmt;// if _userLimit is set check this to find out how many users are present!
	unsigned int				_maxMemAmt;// if _userLimit is set this will contain the max amt of channel members!
	std::string					_modes;
	std::string					_activeMode;

	bool						_inviteOnly;
	bool						_topicOperOnly;
	bool						_userLimit;//use this bool to know if there is a user limit

	std::map<Client*, bool>	_members;// std::stirng for the nick, and bool for operator status!

	public:
	Channel();
	Channel(std::string name);

	std::vector<std::string>	getChannelMembers();
	void						msgChannel(Client& sender, std::string message);
	bool						chkIfMember(std::string user);
	bool						joinChannel(Client& newMember, std::string password);
	void						manipOper(std::string nick, bool promote);
	bool						chkIfOper(std::string nick);
	bool						addFlag(char mode);
	bool						removeflag(char mode);
	std::string					getValues(void);

	void						handleModeO(Client& sender, char mode, bool addflag, std::string param);
	void						handleModeK(Client& sender, char mode, bool addflag, std::string param);
	void						handleModeL(Client& sender, char mode, bool addflag, std::string param);
	void						handleTypeBC(Client& sender, char mode, bool addflag, std::string param);
	void						handleTypeD(char mode, bool addflag);

	void						chanMode(Client& sender, std::string modes, std::string value);

	bool						chkTopic();
	void						geTopic(Client& user);
	void						seTopic(std::string author, std::string topic);
//! tmp
	void	printChan();
};

// create a member funtion to add oper and one to remove
// ig we can just chk _pass.size() to find out if the channel is password protected!
// there seems to have no use of keeping track of topic related flags becuz the pdf says only opers can either view
// or modify it 💀