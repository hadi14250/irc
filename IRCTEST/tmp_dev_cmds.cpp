#include "Server.hpp"

void	Server::addDevs(int fd, std::string dev) {
	Commands	parseDevP(fd, "PASS", _password, _pfdsMap[fd]);
	Commands	parseDevU(fd, "USER", "a a a a", _pfdsMap[fd]);
	Commands	parseDevH(fd, "NICK", dev, _pfdsMap[fd]);
}