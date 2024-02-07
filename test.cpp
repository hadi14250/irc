#include <iostream>
#include <sys/types.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string>
#include <string.h>
#include <sstream>

int main()
{
	std::istringstream ss("544.0");
	int i;

	ss >> i;
	// if (ss.get() != EOF)
	// 	std::cout << "There's still something there at the end\n";
	std::streampos remaining = ss.tellg(); //this gives us the position that ss is at
	std::cout << remaining << "\n";
	std::cout << i << "\n";

}

// int main()
// {
// 	//create a socket
// 	 //AF_INET - ipv4 internet, sockstream = connection based protocol, 1 protocol
// 	int listening = socket(AF_INET, SOCK_STREAM, 0);
// 	if (listening == -1)  
// 	{
// 		std::cerr << "Error: Can't create socket" << std::endl;
// 		return -1;
// 	}

// 	//Bind ip address and port to a socket
// 	sockaddr_in hint; //IPV4 internet domain socket address
// 	//sin_family is protocol family of socket. AF_INET is most common for sackaddr_in
// 	hint.sin_family = AF_INET; 
// 	//sin_port = port number; stored in network byte order (big endian)
// 	//big endian means that most significant byte is stored at lowest memory address (read from left to right)
// 	//htons - host to network short - converts host byte order to network byte order
// 	//port numbers range from 0 - 65535
// 	//0-1023 are rserved for standard service
// 	//1024-49151 registered fro various applications
// 	//just choose something that won't conflict with well known or registerd ports
// 	hint.sin_port = htons(54000);
// 	//convert IPV4 (human readable address) to binary fomat to be used by network related structs like struct sockaddr_in
// 	//so basically convert ipv4 addr "0.0.0.0" into binary format and store in hint.sin_addr;
// 	//0.0.0.0 allows us to get any address
// 	inet_pton(AF_INET, "0.0.0.0", &hint.sin_addr);
// 	//bind socket fd listening to socket address hint
// 	//need to convert pointer type of &hint to sockaddr*
// 	//normally necessary to assign a local address using bind before a SOCK_STREAM socket may receive connections
// 	if (bind(listening, (sockaddr*)&hint, sizeof(hint)) == -1)
// 	{
// 		std::cerr << "Error: can't bind to ip/port" << std::endl;
// 		return -2;
// 	}

// 	//Mark socket for listening in
// 	//SOMAXCONN - constant that represents max length of queue of pending connections for a socket
// 		//this number can vary between systems, so better to just use the constant
// 	if (listen(listening, SOMAXCONN) == -1)
// 	{
// 		std::cerr << "Error: can't listen" << std::endl;
// 		return -3;
// 	}

// 	//Accept a call / Wait for a connection
// 	sockaddr_in client; 
// 	socklen_t clientSize = sizeof(client); 
// 	//NI_MAXHOST - constant that represents max length of a host name string 
// 	char host[NI_MAXHOST];//client's remote name
// 	//NI_MAXSERV - constant that represents max length of a service name string
// 	char service[NI_MAXSERV];//service/port client is connected on

// 	int clientSocket = accept(listening, (sockaddr*)&client, &clientSize);
// 	if (clientSocket == -1)
// 	{
// 		std::cerr << "error: Can't connect with client" << std::endl;
// 		return -4;
// 	}

// 	//empty out host and svc so they are blank spaces. We can also calloc host and svc to save this step
// 	memset(host, 0, NI_MAXHOST);
// 	memset(service, 0, NI_MAXSERV);	

// 	//convert socket address into human readable hostname and servicename
// 	int result = getnameinfo((sockaddr*)&client, sizeof(client), host, NI_MAXHOST, service, NI_MAXSERV, 0);
// 	if (result == 0)
// 		std::cout << host << "conected on " << service << std::endl;
// 	else
// 	{
// 		//if getnameinfo fails, it does not mean that connection failed, just that conversion from binary to human readable info failed
// 		//so instead of using the function, we will manually change the info using inet_ntop and ntohs
// 		//convert binary IP address in client.sin_addr to human readable string and store in buffer host
// 		inet_ntop(AF_INET, &client.sin_addr, host, NI_MAXHOST);
// 		//use ntohs to convert port number from network byte order back to host byte order
// 		std::cout << host << "connected on port " << ntohs(client.sin_port) << std::endl;
// 	}

// 	//close our listening socket
// 	close(listening);

// 	//while receiving disply message, echo message
// 	char buf[4096];
// 	while (true)
// 	{
// 		//clear buffer
// 		memset(buf, 0, 4096);
// 		//wait for message
// 		int bytesRecv = recv(clientSocket, buf, 4096, 0);
// 		if (bytesRecv == -1)
// 		{
// 			std::cerr << "Error: connection issue" << std::endl;
// 			break;
// 		}
// 		if (bytesRecv == 0)
// 		{
// 			std::cout << "client disconnected" << std::endl;
// 			break;
// 		}
// 		//display message. convert buf which is cstring to c++ string
// 		std::cout << "Received: " << std::string(buf, 0, bytesRecv);

// 		//resend message / echo message back to client
// 		send(clientSocket, buf, bytesRecv + 1, 0);		
// 	}

// 	close(clientSocket);

// }

//use nc localhost 54000 to connect on a separate terminal

