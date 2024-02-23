#pragma once

#include <iostream>
#include <map>
#include <string>
#include "Client.hpp"

class Channel {
private:
    std::string _name; // Channel name
    std::string _topic; // Channel topic
    std::string _topicCreation; // Time of topic creation
    std::string _pass; // Password if channel is password protected
    unsigned int _curMemAmt; // Current number of members
    unsigned int _maxMemAmt; // Max number of members allowed
    bool _inviteOnly; // Is the channel invite-only?
    bool _userLimit; // Is there a user limit?

    std::map<std::string, bool> _members; // Nicknames and pointer to Client objects


public:
    Channel(std::string name); // Constructor

	// channel commands
	void join(std::string nick, std::string pass);

    bool chkIfOper(std::string nick); // Check if a user is an operator

	// setters and getters
	
	void			incCurrentAmmount();
	unsigned int	getCurrentAmmount();

	void			setMaxMemAmt(unsigned int maxLimit);
	unsigned int 	getMaxMemAmt();

	void 			setChanName(std::string name);
	std::string 	getChanName();

	void 			setChanTopic(std::string topic);
	std::string 	getChanTopic();

	void 			setTopicCreation(std::string date);
	std::string 	getTopicCreation();

	void 			setChanPass(std::string pass);
	std::string		getChanPass();

	void			setInviteOnly(bool flag);
	bool			getInviteOnly();

	void			setUserLimit(bool flag);
	bool			getUserLimit();
};
