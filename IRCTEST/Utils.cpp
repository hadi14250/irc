#include "Utils.hpp"

std::string removeNl(std::string str) {
	size_t	pos = str.find_last_not_of(" \n\r");
	if (pos != std::string::npos)
		return str.substr(0, pos + 1);
	return str;
}

/* 
	removes first word if multiple words exists!
	return "" if msg empty or no more words exist!
	does not get rid of trailing whitespace!
 */
std::string	removeCmd(std::string msg) {
	size_t	pos = 0;

	pos = msg.find_first_not_of(" ", 0);// find first word! else return ""
	if (pos == std::string::npos)
		return "";
	pos = msg.find(" ", pos);// go past the first word! if nothing else there return ""
	if (pos == std::string::npos)
		return "";
	pos = msg.find_first_not_of(" ", pos);// find next word! if no more return ""
	if (pos == std::string::npos)
		return "";
	return msg.substr(pos, msg.size() - pos);
}

std::string	getCmd(std::string msg) {
	size_t	pos[2] = {0};

	pos[0] = msg.find_first_not_of(" ", 0);// find index of first letter! else no words exist!
	if (pos[0] == std::string::npos)
		return "";// msg is just filled with whitespaces! 
	pos[1] = msg.find_first_of(" ", pos[0]);// find index of last letter
	if (pos[1] == std::string::npos)// no other words nor whitespace exist which means everthing else is the word!
		return &msg[pos[0]];
	return removeNl(msg.substr(pos[0], pos[1] - pos[0]));
}

size_t	chkArgs(std::string args, size_t limiter);

std::vector<std::string>	splitPlusPlus(std::string str, std::string del) {
	std::vector<std::string>	split;
	std::size_t	i[2] = {0};

	if (str.empty() || del.empty()) {
		if (del.empty())
			split.push_back(str);
		return split;
	}
	while (str.size() && i[0] < str.size()) {
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
/* 
	chks how many arguments the string contains and returns the amount!
	if limiter is not zero then it means u just trying to find out if the string
	contains an amount of arg and don't care about the extra this will make the
	funtion look for only an amount and then just return that value or less if the
	str contains less that asked for!
 */
size_t	chkArgs(std::string args, size_t limiter) {
	size_t		i;

	if (args.size()) {
		if (args.find_first_not_of(" ") == std::string::npos)
			return 0;
		else
			args = args.substr(args.find_first_not_of(" "), args.size() - args.find_first_not_of(" "));
	}
	for (i = 0; !args.empty() && i < (limiter ? limiter : 69) ; i++)
		args = removeCmd(args);
	return i;
}

std::string	removeColon(std::string str) {
	if (str.size() && str.at(0) == ':')
		str = str.substr(1);
	return (str);
}
