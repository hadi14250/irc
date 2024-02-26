#pragma once

#include <map>
#include <iostream>

#include "Server.hpp"
#include "Channel.hpp"
#include "Client.hpp"
#include <algorithm>

class Client;

typedef std::map<Client*, bool>::iterator channelMemIt;

class Channel {
	private:
	std::string					_name; //channel name just in case!
	std::string					_topic;
	std::string					_topiCreation;// time of topic creation!
	std::string					_pass;//password if channel is password protected!
	unsigned int				_curMemAmt;// if _userLimit is set check this to find out how many users are present!
	unsigned int				_maxMemAmt;// if _userLimit is set this will contain the max amt of channel members!

	bool						_inviteOnly;
	bool						_userLimit;//use this bool to know if there is a user limit

	std::map<Client*, bool>	_members;// std::stirng for the nick, and bool for operator status!

	public:
	Channel();
	Channel(std::string name);

	void	msgChannel(Client& sender, std::string message);
	bool	chkIfmember(std::string user);
	void	joinChannel(Client& newMember, std::string password);
	bool	chkIfOper(std::string nick);

};

// create a member funtion to add oper and one to remove
// ig we can just chk _pass.size() to find out if the channel is password protected!
// there seems to have no use of keeping track of topic related flags becuz the pdf says only opers can either view
// or modify it 💀