//delete from any channels -> delete this first!
	//get client address (&address)
	//get list of channels client belongs to in client object
	//go into each channel and delete the client from each channel
//delete from nickMap
	//get client nicknam
	//delete from nickMap
//delete from pfdsMap
//set change to 1 somehow?
//make sure there's nothing to free in the client object!
//need to send :sender_id QUIT :goodbye message -> 
	//should we send it to other users in the channel? -> we need to do this if client exits without quit command






/* FOR FILE TRANSFER BONUS

use Client._clientInfo to access port num and ip

struct sockaddr_in {
    short int          sin_family;  // Address family, AF_INET
    unsigned short int sin_port;    // Port number
    struct in_addr     sin_addr;    // Internet address
    unsigned char      sin_zero[8]; // Same size as struct sockaddr
};

struct in_addr {
    uint32_t s_addr; // that's a 32-bit int (4 bytes)
};

I THINK you need to use ntohs function to convert port and ntohl function to conver the address
since they would currently be in network byte order

*/