#include "../hpp/WebServer.hpp"



void WebServer::initializeSockets(std::vector<std::pair<ServerConfig, std::map<std::string, ServerConfig*> > > &serversConfig,
				std::vector<Server> &serversVec, std::map<int, Server*> &ServersMap, MimeAndError *mime)
{
	size_t i = 0;
	size_t confSize = serversConfig.size();

	for (; i < confSize; ++i)
	{
		serversVec[i].setMimeError(mime);
		serversVec[i].setConfigData(&serversConfig[i].first);
		serversVec[i].setConfigDataServerNames(&serversConfig[i].second);

		serversVec[i].socketBindListen();
		ServersMap[serversVec[i].getFdSock()] = &serversVec[i];
	}
}

void	WebServer::run(std::vector<std::pair<ServerConfig, std::map<std::string, ServerConfig*> > >&serversConfig)
{
	MimeAndError 						mime;
	std::map<int, Server*> 				ServersMap;
	std::map<int, Server*>::iterator	itServer;
	fd_set              				readSet;
    fd_set              				writeSet;


	std::vector<Server> servers(serversConfig.size());
	initializeSockets(serversConfig, servers, ServersMap, &mime);


	FD_ZERO(&readSet);
    FD_ZERO(&writeSet);

	for (itServer = ServersMap.begin(); itServer != ServersMap.end(); ++itServer)
    {
		const int	&fdServer = itServer->first;
		FD_SET(fdServer, &readSet);
	}
	struct timeval timeout;
	timeout.tv_sec = 120;
	timeout.tv_usec =0;
	while (true)
	{
		itServer = --(ServersMap.end());
		int	maxSocket = itServer->first;

		for (itServer = ServersMap.begin(); itServer != ServersMap.end(); ++itServer)
			maxSocket = std::max(maxSocket, itServer->second->maxSockClient());

		fd_set	tempReadSet = readSet;
		fd_set	tempWriteSet = writeSet;

		int selectAnswer = select(maxSocket + 1, &tempReadSet, &tempWriteSet, 0, &timeout);
		if (selectAnswer == -1)
		{
			perror("Select");
			exit(1);
		}
		else if (selectAnswer)
		{ 
			for (itServer = ServersMap.begin(); itServer != ServersMap.end(); ++itServer)
			{
				const int &fdServer = itServer->first;
				if (FD_ISSET(fdServer, &tempReadSet))
					itServer->second->acceptClient(readSet);
			}

			for (itServer = ServersMap.begin(); itServer != ServersMap.end(); ++itServer)
				itServer->second->processReadySockets(tempReadSet, tempWriteSet, readSet, writeSet);
		}
		else
		{
			for (itServer = ServersMap.begin(); itServer != ServersMap.end(); ++itServer)
				itServer->second->checkClientsTimeout(readSet, writeSet);
		}
	}
}
