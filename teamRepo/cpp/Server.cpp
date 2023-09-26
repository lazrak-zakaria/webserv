#include "../hpp/Server.hpp"


void	Server::setConfigData(ServerConfig	*c)
{
	configData = c;
}

void	Server::setMimeError(MimeAndError	*m)
{
	mimeError = m;
}

int		Server::getFdSock()
{
	return fdSock;
}

void	Server::socketBindListen()
{
	fdSock = socket(AF_INET, SOCK_STREAM, 0);
    if (fdSock == -1)
    {
        perror("socket");
        exit(1);
    }
    bzero(&addrServer, sizeof(addrServer));
    addrServer.sin_family = AF_INET;
	// https://stackoverflow.com/questions/16508685/understanding-inaddr-any-for-socket-programming
    addrServer.sin_addr.s_addr = INADDR_ANY;
    addrServer.sin_port = htons(configData->port);
	int optval = 1;
	setsockopt(fdSock, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof (optval));
    if (bind(fdSock, (struct sockaddr *) &addrServer,
              sizeof(addrServer)) < 0) 
    {
        perror("bind");
        exit(1);
    }
    listen(fdSock, 1000); // change the 10 later;
}

int	Server::maxSockClient() const
{
	if (serverClients.empty()) return -1;
	std::map<int, Client>::const_iterator	it;
	it = serverClients.end();
	--it;
	return (it->first);
}



void	Server::acceptClient(fd_set &readSet)
{

	struct sockaddr_in  clientSockaddr;
	socklen_t			clientSockaddrLength ;
	int					fdSockTmp;

	memset(&clientSockaddr, 0, sizeof(clientSockaddr));
	clientSockaddrLength = 0;
	fdSockTmp = accept(fdSock, (struct sockaddr*)&clientSockaddr,  &clientSockaddrLength);
	if (fdSockTmp == -1)
	{
		std::cerr << "connetion error\n";
        return ;
	}
	FD_SET(fdSockTmp, &readSet);
	serverClients[fdSockTmp];
	serverClients[fdSockTmp].setConfigData(configData);
	serverClients[fdSockTmp].setMimeError(mimeError);
}


void	Server::processReadySockets(fd_set &tempReadSet, 
						fd_set &tempWriteSet,fd_set &readSet, fd_set &writeSet)
{
	std::map<int , Client>::iterator it;
	for (it = serverClients.begin(); it != serverClients.end(); ++it)
	{
		const int	&clientFdSock = it->first;
		Client		&clientObj = it->second;

		if (FD_ISSET(clientFdSock, &tempReadSet))
		{
			char	buf[10000];
			int		collected = recv(clientFdSock, buf, 9999, 0);

			if (collected == -1 || collected == 0)
			{
				invalidSockets.insert(clientFdSock);
				FD_CLR(clientFdSock, &readSet);
				std::cout << "drop client\n";
				continue ;
			}
			
			
			clientObj.readRequest(buf, collected);


			if (clientObj.isRequestFinished())
			{
				FD_CLR(clientFdSock, &readSet);
				FD_SET(clientFdSock, &writeSet);
			}
		}
		else if (FD_ISSET(clientFdSock, &tempWriteSet))
		{
			std::string &answer = clientObj.serveResponse();

			if(client_obj.isRespo)
			{
				answer.push_back('\r');
				answer.push_back('\n');
			}
		}
	}
	for (std::set<int>::iterator it = invalidSockets.begin(); it != invalidSockets.end(); ++it)
	{
		close(*it);
		serverClients.erase(*it);
	}
}