#include "Channel.hpp"

Channel::Channel() :
						_name(""),
						_topic(""),
						_topiCreation(""),
						_pass(""),
						_curMemAmt(0),
						_maxMemAmt(0),
						_inviteOnly(false),
						_userLimit(false)
{}

Channel::Channel(std::string name) :
						_name(name),
						_topic(""),
						_topiCreation(""),
						_pass(""),
						_curMemAmt(0),
						_maxMemAmt(0),
						_inviteOnly(false),
						_userLimit(false)
{
	Server::_chanMap[name] = *this;
}

bool	Channel::chkIfmember(std::string user) {
	channelMemIt	it;
	for (it = _members.begin(); (it != _members.end()) && it->first->_nick.compare(user); it++);
	return (it == _members.end() ? false : true);
}

bool	Channel::chkIfOper(std::string nick) {
	channelMemIt	it;
	for (it = _members.begin(); (it != _members.end()) && it->first->_nick.compare(nick); it++);
	return (it == _members.end() ? false : it->second);
}

void	Channel::joinChannel(Client& newMember, std::string password) {
	vecStrIt		it;

	if (chkIfmember(newMember._nick))
		std::cerr << "err";// ERR_USERONCHANNEL
	else if (_userLimit && (_curMemAmt >= _maxMemAmt))
		std::cerr << "err";// ERR_CHANNELISFULL
	else if (_inviteOnly && ((it = find(newMember._invitations.begin(), newMember._invitations.end(), _name)) == newMember._invitations.end()))
		std::cerr << "err";// ERR_INVITEONLYCHAN
	else if (_pass.size() && _pass.compare(password))
		std::cerr << "err";// ERR_BADCHANNELKEY
	else {
		if (_inviteOnly)
			newMember._invitations.erase(it);  
		newMember._channels.push_back(_name);
		_members[&newMember] = false;
	}// should send a message to the client showing him that he joined the channel!
}

void	Channel::msgChannel(std::string message) {
	channelMemIt	it = _members.begin();

	for (; it != _members.end() && ; it++) 
		Server::_pfdsMap[it->first->_sockfd]._messages.push_back(message + "\r\n");
}



/* 
	msg -> send messages to anything
	msg -> 
	aa
 */
