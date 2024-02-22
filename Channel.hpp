#pragma once

#include <iostream>
#include <map>
#include "Client.hpp"

class Channel {
private:
	std::string					_name; //channel name just in case!
	std::string					_topic;
	std::string					_topiCreation;// time of topic creation!
	std::string					_pass;//password if channel is password protected!
	unsigned int				_curMemAmt;// if _userLimit is set check this to find out how many users are present!
	unsigned int				_maxMemAmt;// if _userLimit is set this will contain the max amt of channel members!

	bool						_inviteOnly;
	bool						_userLmit;

	// std::map<std::string, bool>	_members;// std::stirng for the nick, and bool for operator status!
	std::map<Client *, bool>	_members;
public:
	Channel();
};

// ig we can just chk _pass.size() to find out if the channel is password protected!
// there seems to have no use of keeping track of topic related flags becuz the pdf says only opers can either view
// or modify it 💀