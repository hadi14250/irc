#include "Server.hpp"

int main(int argc, char **argv){
	if (argc != 3)
	{
		std::cout << "Usage: ./irc <port> <password>\n";
		// Server::printPasswordPolicy();
		return (1);
	}
	try
	{
		Server::setSignals();
		Server server(argv[1], argv[2]);
		server.createServer();
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
	}
}

/* BUGS:
JOIN - if password is set, we cannot join channel even with correct password - Bad Channel Mask
Redo read messages in server
check that when client leaves the client object is gone 
are we deleting client after client quits unexpectedly (^C)
*/
