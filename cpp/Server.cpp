#include "../hpp/Server.hpp"


const Server& Server::operator =(const Server& s)
{
	fdSock = s.fdSock;
	addrServer = s.addrServer;
	configData = s.configData;
	serverNamesConfig = s.serverNamesConfig;
	serverClients = s.serverClients;
	mimeError = s.mimeError;
	invalidSockets = s.invalidSockets;
	return *this;
}

Server::Server()
{
}

Server::Server(const Server& s)
{
	*this = s;
}

void	Server::setConfigData(ServerConfig	*c)
{
	configData = c;
}

void	Server::setConfigDataServerNames(std::map<std::string, ServerConfig*>	*s)
{
	serverNamesConfig = s;
}

void	Server::setMimeError(MimeAndError	*m)
{
	mimeError = m;
}

int		Server::getFdSock()
{
	return fdSock;
}

// https://stackoverflow.com/questions/16508685/understanding-inaddr-any-for-socket-programming
void	Server::socketBindListen()
{
	fdSock = socket(AF_INET, SOCK_STREAM, 0);
    if (fdSock == -1)
    {
        perror("socket");
        exit(1);
    }
	fcntl(fdSock, F_SETFL, O_NONBLOCK, FD_CLOEXEC);
    bzero(&addrServer, sizeof(addrServer));
    addrServer.sin_family = AF_INET;
    // addrServer.sin_addr.s_addr = inet_addr(configData->host.c_str());
	if (!inet_aton(configData->host.c_str(), &addrServer.sin_addr))
	{
        std::cerr << "the address is not valid\n";
        exit(1);
	}
    addrServer.sin_port = htons(configData->port);
	int optval = 1;
	setsockopt(fdSock, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof (optval));
    if (bind(fdSock, (struct sockaddr *) &addrServer,
              sizeof(addrServer)) < 0) 
    {
        perror("bind");
        exit(1);
    }
    if (listen(fdSock, SOMAXCONN))
	{
		perror("listen");
        exit(1);
	}
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

	std::cerr << "accept new client ------------------------------------------>:" << fdSockTmp << "\n";
	FD_SET(fdSockTmp, &readSet);
	serverClients[fdSockTmp];
	serverClients[fdSockTmp].setAllConfigData(serverNamesConfig);
	serverClients[fdSockTmp].setDefaultConfigData(configData);
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

			char	buf[8192];
			int		collected = recv(clientFdSock, buf, 8192, 0);

			if (collected == -1 || collected == 0)
			{
				invalidSockets.push_back(clientFdSock);
				FD_CLR(clientFdSock, &readSet);
				std::cerr << "drop READ client\n";
				continue ;
			}
			
			clientObj._timeLastAction = time(NULL);
			clientObj.readRequest(buf, collected);


			if (clientObj.isRequestFinished())
			{
				FD_CLR(clientFdSock, &readSet);
				FD_SET(clientFdSock, &writeSet);
			}
		}
		else if (FD_ISSET(clientFdSock, &tempWriteSet))
		{
			clientObj._timeLastAction = time(NULL);
			if (clientObj.isCompletelySent)
			{
				clientObj.serveResponse();

				if(clientObj.isResponseFinished())
					clientObj._finalAnswer.append("\r\n");
			}
			int dataSent = send(clientFdSock, clientObj._finalAnswer.c_str(), clientObj._finalAnswer.size(), MSG_NOSIGNAL);

			if (dataSent == -1)
			{
				invalidSockets.push_back(clientFdSock);
				FD_CLR(clientFdSock, &writeSet);
				std::cerr << "drop WRITE client\n";
				continue ;
			}

			if (clientObj._finalAnswer.size() != (size_t)dataSent)
			{
				clientObj._finalAnswer.erase(0, dataSent);
				clientObj.isCompletelySent = false;
			}
			else
				clientObj.isCompletelySent = true;
			if (clientObj.isResponseFinished() && clientObj.isCompletelySent)
			{
				FD_CLR(clientFdSock, &writeSet);
				clientObj.clearClient();
				if (clientObj.closeMe)
				{
					invalidSockets.push_back(clientFdSock);
					continue;
				}
				FD_SET(clientFdSock, &readSet);
			}
		}
		else
		{
			if (time(NULL) - clientObj._timeLastAction > 120)
			{
				invalidSockets.push_back(clientFdSock);
				FD_CLR(clientFdSock, &writeSet);
				FD_CLR(clientFdSock, &readSet);
				std::cerr << "drop select timeout client\n";
				continue ;
			}
		}
	}

	for (std::vector<int>::iterator it = invalidSockets.begin(); it != invalidSockets.end(); ++it)
	{
		std::cerr << "close "<< *it << "\n";
		close(*it);
		serverClients.erase(*it);
	}
	invalidSockets.clear();
}

void	Server::checkClientsTimeout(fd_set &readSet, fd_set &writeSet)
{
	std::map<int , Client>::iterator it;
	for (it = serverClients.begin(); it != serverClients.end(); ++it)
	{
		const int	&clientFdSock = it->first;
		Client		&clientObj = it->second;
	
		if (time(NULL) - clientObj._timeLastAction > 120)
		{
			invalidSockets.push_back(clientFdSock);
			FD_CLR(clientFdSock, &writeSet);
			FD_CLR(clientFdSock, &readSet);
			std::cerr << "drop timeout client\n";
			continue ;
		}
	}
	for (std::vector<int>::iterator it = invalidSockets.begin(); it != invalidSockets.end(); ++it)
	{
		std::cerr << "close "<< *it << "\n";
		close(*it);
		serverClients.erase(*it);
	}
	invalidSockets.clear();
}
