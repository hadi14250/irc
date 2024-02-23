#include "Channel.hpp"

Channel::Channel(std::string name) : _name(name), _topic(""), _topicCreation(""), _pass(""), _curMemAmt(0), _maxMemAmt(0), _inviteOnly(false), _userLimit(false) {}

bool Channel::chkIfOper(std::string nick) {
    std::map<std::string, bool>::iterator channelMemIt = _members.find(nick);
    if (channelMemIt == _members.end()) {
        return false;
    }
    return channelMemIt->second;
}

void			Channel::incCurrentAmmount()
{
	this->_curMemAmt++;
}

unsigned int	Channel::getCurrentAmmount()
{
	return (this->_curMemAmt);
}

void			Channel::setMaxMemAmt(unsigned int maxLimit)
{
	this->_maxMemAmt = maxLimit;
}


unsigned int Channel::getMaxMemAmt()
{
	return (this->_maxMemAmt);
}

void 			Channel::setChanName(std::string name)
{
	this->_name = name;
}

std::string 	Channel::getChanName()
{
	return (this->_name);
}

void Channel::setChanTopic(std::string topic)
{
	this->_topic = topic;
}

std::string 	Channel::getChanTopic()
{
	return (this->_topic);
}

void	Channel::setTopicCreation(std::string date)
{
	this->_topicCreation = date;
}

std::string 	Channel::getTopicCreation()
{
	return (this->_topicCreation);
}

void	Channel::setChanPass(std::string pass)
{
	this->_pass = pass;
}

std::string		Channel::getChanPass()
{
	return (this->_pass);
}

void	Channel::setInviteOnly(bool flag)
{
	this->_inviteOnly = flag;
}

bool	Channel::getInviteOnly()
{
	return (this->_inviteOnly);
}

void	Channel::setUserLimit(bool flag)
{
	this->_userLimit = flag;
}

bool	Channel::getUserLimit()
{
	return (this->_userLimit);
}
