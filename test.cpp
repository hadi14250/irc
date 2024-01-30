#include <iostream>
#include <sys/types.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string>
#include <string.h>

int main()
{
	//create a socket
	int listening = socket(AF_INET, SOCK_STREAM, 0); //AF_INET - ipv4 internet, sockstream = connection based protocol, 1 protocol
	if (listening == -1)
	{
		std::cerr << "Error: Can't create socket" << std::endl;
		return -1;
	}

	//Bind ip address and port to a socket
	sockaddr_in hint; //
}
