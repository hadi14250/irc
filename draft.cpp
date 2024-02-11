void	Server::createServer()
{
	makeListenSockfd();
	while (true)
	{
		int change = 0;
		int pollCount = poll(_pfds, _pfdsCount, -1);
		if (pollCount == -1)
			throw Error();
		for (int i = 0; i < _pfdsCount; i++)
		{
			if (_pfds[i].fd == _listenSockfd && (_pfds[i].revents & POLLIN))
			{
				try
				{
					std::cout << "attempting to add a new client\n";
					addNewPfd(CLIENTFD);
					change = 1;
					std::cout << "finished adding newpfd\n";
				}
				catch(const std::exception& e)
				{
					std::cerr << e.what() << '\n';
				}
			}
			else if (_pfds[i].revents & POLLIN) 
			{
				char buf[510] = {};
				int nbytes = recv(_pfds[i].fd, buf, sizeof(buf), 0);
				if (nbytes <= 0)
				{
					deletePfd(_pfds[i].fd, nbytes);
					change = 1;
				}
				else
					std::cout << "we execute something here\n";
			}
		}
			if (change == 1)
			{
				std::cout << "copying array\n";
				copyPfdMapToArray();
			}
	}
	freeaddrinfo(_serv);
}