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

Commands::QUIT()
{
	//is it possible to make the list of channels in clients a pointer to the actual channel?
	std::vector<std::string> chanList = _sender._channels;
	for (std::vector<std::string>::iterator it = chanList.begin(); it != chanList.end(); it++)
		*it->_members.erase(&client);
	Server::_nickMap.erase(_sender._nick);
	Server::_pfdsMap.erase(_sender);
	Server::_change = 1;
}