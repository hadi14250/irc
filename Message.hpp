#pragma once

# include <string>
# include <vector>

class Message
{
public:
	int							_fd;
	std::string 				_command;
	std::vector<std::string>	_param;
	std::string					_sender;
	std::string					_recevier;
};