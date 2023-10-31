#include "../hpp/WebServer.hpp"



void WebServer::initializeSockets(std::vector<std::pair<ServerConfig*, std::map<std::string, ServerConfig*> > > &serversConfig,
				std::vector<Server> &serversVec, std::map<int, Server*> &ServersMap, MimeAndError *mime)
{
	size_t i = 0;
	size_t confSize = serversConfig.size();

	for (; i < confSize; ++i)
	{
		serversVec[i].setMimeError(mime);
		serversVec[i].setConfigData(serversConfig[i].first);
		serversVec[i].setConfigDataServerNames(&serversConfig[i].second);

		serversVec[i].socketBindListen();
		ServersMap[serversVec[i].getFdSock()] = &serversVec[i];
	}
}


#ifndef _KQUEUE_

void	WebServer::run(std::vector<std::pair<ServerConfig*, std::map<std::string, ServerConfig*> > >&serversConfig)
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

#else
void	WebServer::run(std::vector<std::pair<ServerConfig*, std::map<std::string, ServerConfig*> > >&serversConfig)
{
	MimeAndError 						mime;
	std::map<int, Server*> 				ServersMap;
	std::map<int, Server*>::iterator	itServer;

	struct kevent	evSet;
	int				kq;

	std::vector<Server> servers(serversConfig.size());
	initializeSockets(serversConfig, servers, ServersMap, &mime);

	kq = kqueue();

	for (itServer = ServersMap.begin(); itServer != ServersMap.end(); ++itServer)
    {
		const int	&fdServer = itServer->first;

		EV_SET(&evSet, fdServer, EVFILT_READ, EV_ADD, 0, 0, NULL);
    	kevent(kq, &evSet, 1, NULL, 0, NULL);
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


		struct kevent *eventList = new struct kevent[maxSocket];


		int numEvents = kevent(kq, NULL, 0, eventList, maxSocket, NULL);
		if (numEvents == -1)
		{
			perror("Kqueue");
			exit(1);
		}

		for (int i = 0; i < numEvents; ++i)
		{
			if (ServersMap.count(eventList[i].ident))
			{
				ServersMap[eventList[i].ident]->acceptClient(kq);
			}
			else if (eventList[i].filter == EVFILT_READ || eventList[i].filter == EVFILT_WRITE
					|| (eventList[i].filter & EV_EOF))
			{
                for (itServer = ServersMap.begin(); itServer != ServersMap.end(); ++itServer)
				{
					if (itServer->second->isClientExist(eventList[i].ident))
					{
						itServer->second->processReadySockets(kq, eventList[i]);
						break;
					}
				}
            }
		}

		delete []eventList;
	}
}
#endif
