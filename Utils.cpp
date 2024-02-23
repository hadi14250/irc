#include "Server.hpp"

std::string removeNl(std::string str) {
	if (str.find("\r\n") != std::string::npos)
		return str.substr(0, str.size() - 2);
	size_t	pos = str.find_last_not_of(" \n\r");
	if (pos != std::string::npos)
		return str.substr(0, pos + 1);
	return str;
}

std::string	removeCmd(std::string msg) {
	size_t	pos[2] = {0};

	pos[0] = msg.find(" ", 0);
	if (pos[0] == std::string::npos)
		return removeNl(msg);
	pos[1] = msg.find_first_not_of(" ", pos[0]);
	if (pos[1] == std::string::npos)
		return removeNl(&msg[pos[0]]);
	return removeNl(&msg[pos[1]]);
}

std::string	getCmd(std::string msg) {
	size_t	pos = 0;

	pos = msg.find(" ", 0);
	if (pos == std::string::npos)
		return removeNl(msg);
	return msg.substr(0, pos);
}

std::vector<std::string>	splitPlusPlus(std::string str, std::string del) {
	std::vector<std::string>	split;
	std::size_t	i[2] = {0};

	while (i[0] < str.size()) {
		i[1] = str.find_first_of(del, i[0]);
		if (i[1] != std::string::npos) {
			if (i[0] == i[1])
				split.push_back("");
			else
				split.push_back(str.substr(i[0], i[1] - i[0]));
		}
		else {
			split.push_back(removeNl(str.substr(i[0])));
			break ;
		}
		i[0] = i[1] + 1;
	}
	return split;
}

//! new, I'll add this to every command parser! not included at the moment!

std::string	removeColon(std::string str) {
	if (str.at(0) == ':')
		str = str.substr(1);
	return (str);
}