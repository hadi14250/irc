#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string>
#include <iostream>

int main()
{
	//1. set up addrinfo struct for usage later
	int status;
	struct addrinfo hints;
	struct addrinfo *servinfo;

	memset(&hints, 0, sizeof(hints)); //set hints to 0
	hints.ai_family = AF_UNSPEC; //could be ipv4 or ipv6
	hints.ai_socktype = SOCK_STREAM; //TCP stream sockets
	hints.ai_flags = AI_PASSIVE; //assign address of local host to socket structures

	//NULL bc we don't have a specific IP address. otherwise we set same IP address in ai_flags and first parameter
	//servinfo will be turned into a linked list pointing to 1+ struct addrinfos
	//hints is used to create servinfo basically
	//use port 3490
	status = getaddrinfo(NULL, "3490", &hints, &servinfo);

	if (status != 0)
	{
		//use gai_strerror - we'll look at this again later
		std::cout << "error\n";
		exit(1);
	}

	//1.5. go through servinfo linked list to look for valid entries
		//CHECK LATER

	//2. get a socket file descriptor! (FOR NOW WE ASSUME THE FIRST ONE IS CORRECT)
	int sockfd;
	sockfd = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol);
	if (sockfd == -1)
	{
		std::cout << "error\n";
		exit(1);
	}

	//3. bind socket to a port (associate your socket fd to a port on your local machine)
	//we bind to port we passed in getaddrinfo()
	if (bind(sockfd, servinfo->ai_addr, servinfo->ai_addrlen) == -1)
	{
		std::cout << "error\n";
		exit(1);
	}





		

	


}