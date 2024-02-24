#include "../Client.hpp"

///////////////////////////////////// tmp ////////////////////////////////////////
void	printMsg(std::string msg) { std::cout << "<" << msg << ">" << std::endl; }
//////////////////////////////////////////////////////////////////////////////////

std::string	removeCmd(std::string msg) {
	size_t	pos[2] = {0};

	pos[0] = msg.find(" ", 0);
	if (pos[0] == std::string::npos)
		return msg;
	pos[1] = msg.find_first_not_of(" ", pos[0]);
	if (pos[1] == std::string::npos)
		return &msg[pos[0]];
	return (&msg[pos[1]]);
}

void	Server::cap(int fd, std::string msg) {
	if (!msg.compare("LS") || !msg.compare("LS "))
		this->_pfdsMap[fd]._messages.push_back("CAP * LS :");
	else if (!msg.compare("LIST") || !msg.compare("LIST "))
		this->_pfdsMap[fd]._messages.push_back("CAP * LIST :");
	//chk for other cap cmds if someone tries to gnome u!
	return ;
}

void	Server::parseMsg(int fd, std::string msg) {
	printMsg(msg);
	if (!msg.compare("CAP") || !msg.compare(!69, 4, "CAP "))
		cap(fd, removeCmd(msg));
	// else if (!msg.compare("NICK") || !msg.compare("NICK "))
	// 	nick(fd, removeCmd(msg));
	// else if (!msg.compare("USER") || !msg.compare("USER "))
	// 	user(fd, removeCmd(msg));
	// else if (!msg.compare("PASS") || !msg.compare("PASS "))
	// 	pass(fd, removeCmd(msg));
	else
		std::cerr << "da fag is this!" << std::endl;
}
