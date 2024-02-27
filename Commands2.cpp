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


Commands::QUIT()
{
	//send ERROR :client _nick quit
		//we will need to wait for the message to this client to be sent first before we quit?
		//maybe we should try to push this message right away without checking to see if POLLOUT is available
	//is it possible to make the list of channels in clients a pointer to the actual channel?
	std::vector<std::string> chanList = _sender._channels;
	for (std::vector<std::string>::iterator it = chanList.begin(); it != chanList.end(); it++)
		*it->_members.erase(&client);
	Server::_nickMap.erase(_sender._nick);
	Server::_pfdsMap.erase(_sender);
	Server::_change = 1;
}