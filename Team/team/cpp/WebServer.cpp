#include "../hpp/WebServer.hpp"

void	WebServer::run(std::map<int, Server*> &serversConfig)
{
	std::map<int, Server*>::iterator	itServer;
	fd_set              				readSet;
    fd_set              				writeSet;

	FD_ZERO(&readSet);
    FD_ZERO(&writeSet);

	for (itServer = serversConfig.begin(); itServer != serversConfig.end(); ++itServer)
    {
		const int	&fdServer = itServer->first;
		FD_SET(fdServer, &readSet);
	}

	while (true)
	{
		itServer = --(serversConfig.end());
		int	maxSocket = itServer->first;

		for (itServer = serversConfig.begin(); itServer != serversConfig.end(); ++itServer)
			maxSocket = std::max(maxSocket, itServer->second->maxSockClient());

		fd_set	tempReadSet = readSet;
		fd_set	tempWriteSet = writeSet;

		if (select(maxSocket + 1, &tempReadSet, &tempWriteSet, 0, 0) == -1)
		{
			perror("Select");
			exit(1);
		}

		for (itServer = serversConfig.begin(); itServer != serversConfig.end(); ++itServer)
		{
			const int &fdServer = itServer->first;
			if (FD_ISSET(fdServer, &tempReadSet))
				itServer->second->acceptClient(readSet);
		}

		for (itServer = serversConfig.begin(); itServer != serversConfig.end(); ++itServer)
			itServer->second->processReadySockets(tempReadSet, tempWriteSet, readSet, writeSet);
	}
}
