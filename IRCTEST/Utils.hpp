#pragma once

# include <vector>
# include <iostream>
# include <string>

std::string					removeNl(std::string str);
std::string					removeCmd(std::string msg);
std::string					getCmd(std::string msg);
std::vector<std::string>	splitPlusPlus(std::string str, std::string del);
size_t						chkArgs(std::string args, size_t limiter);
std::string					removeColon(std::string str);