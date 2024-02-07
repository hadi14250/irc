#include "Server.hpp"

int main(int argc, char **argv){
	if (argc != 3)
	{
		std::cout << "Usage: ./ircserv <port> <password>\n";
		Server::printPasswordPolicy();
	}
	
	try
	{
		Server server(argv[1], argv[2]);
	}
	catch(Server::InvalidPasswordException())
	{
		Server::printPasswordPolicy();
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
	}
	
}