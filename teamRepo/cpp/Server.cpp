#include "../hpp/Server.hpp"


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


#ifndef _KQUEUE_

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

	std::cout << "accept new client ------------------------------------------>:" << fdSockTmp << "\n";
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
				std::cout << "drop READ client\n";
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
				// if (!clientObj._finalAnswer.empty())//
				//std::cout << "||||||" <<clientObj._finalAnswer<< "|||||||||||\n";

				if(clientObj.isResponseFinished())
					clientObj._finalAnswer.append("\r\n");
			}
			
			int dataSent = send(clientFdSock, clientObj._finalAnswer.c_str(), clientObj._finalAnswer.size(), 0);

			if (dataSent == -1)
			{
				invalidSockets.push_back(clientFdSock);
				FD_CLR(clientFdSock, &writeSet);
				std::cout << "drop WRITE client\n";
				continue ;
			}

			if (clientObj._finalAnswer.size() != dataSent)
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
				std::cout << "drop timeout client\n";
				continue ;
			}
		}
	}

	for (std::vector<int>::iterator it = invalidSockets.begin(); it != invalidSockets.end(); ++it)
	{
		std::cout << "close "<< *it << "\n";
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
			std::cout << "drop timeout client\n";
			continue ;
		}
	}
	for (std::vector<int>::iterator it = invalidSockets.begin(); it != invalidSockets.end(); ++it)
	{
		std::cout << "close "<< *it << "\n";
		close(*it);
		serverClients.erase(*it);
	}
	invalidSockets.clear();
}


#else

bool	Server::isClientExist(int fd)
{
	return serverClients.count(fd);
}

void	Server::acceptClient(int kq)
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

	std::cout << "accept new client ------------------------------------------>:" << fdSockTmp << "\n";
	struct kevent evSet;
	EV_SET(&evSet, fdSockTmp, EVFILT_READ, EV_ADD, 0, 0, NULL);
    kevent(kq, &evSet, 1, NULL, 0, NULL);
	serverClients[fdSockTmp];
	serverClients[fdSockTmp].setAllConfigData(serverNamesConfig);
	serverClients[fdSockTmp].setDefaultConfigData(configData);
	serverClients[fdSockTmp].setMimeError(mimeError);
}


void	Server::processReadySockets(int kq, struct kevent& evList)
{

	const int	&clientFdSock = evList.ident;
	Client		&clientObj = serverClients[evList.ident];

	if (evList.filter == EVFILT_READ)
		{

			char	buf[8192];
			int		collected = recv(clientFdSock, buf, 8192, 0);

			if (collected == -1 || collected == 0)
			{
				invalidSockets.push_back(clientFdSock);
				deleteRead(kq, evList);
				std::cout << "drop READ client\n";
			}
			else
			{
				clientObj._timeLastAction = time(NULL);
				clientObj.readRequest(buf, collected);


				if (clientObj.isRequestFinished())
				{
					changeToWrite(kq, evList);
				}
			}
		}
		else if (evList.filter == EVFILT_WRITE)
		{
			clientObj._timeLastAction = time(NULL);
			if (clientObj.isCompletelySent)
			{
				clientObj.serveResponse();
				// if (!clientObj._finalAnswer.empty())//
				//std::cout << "||||||" <<clientObj._finalAnswer<< "|||||||||||\n";

				if(clientObj.isResponseFinished())
					clientObj._finalAnswer.append("\r\n");
			}
			
			int dataSent = send(clientFdSock, clientObj._finalAnswer.c_str(), clientObj._finalAnswer.size(), 0);

			if (dataSent == -1)
			{
				invalidSockets.push_back(clientFdSock);
				deleteWrite(kq, evList);
				std::cout << "drop WRITE client\n";

			}
			else
			{
				if (clientObj._finalAnswer.size() != dataSent)
				{		
					clientObj._finalAnswer.erase(0, dataSent);
					
					clientObj.isCompletelySent = false;
				}
				else
					clientObj.isCompletelySent = true;
				if (clientObj.isResponseFinished() && clientObj.isCompletelySent)
				{
					clientObj.clearClient();
					if (clientObj.closeMe)
					{
						deleteWrite(kq, evList);
						invalidSockets.push_back(clientFdSock);
					}
					else
						changeToRead(kq, evList);
				}
			}
		}
		else if (evList.filter & EV_EOF)
		{
			invalidSockets.push_back(clientFdSock);
			if (clientObj.isRequestFinished())
				deleteWrite(kq, evList);
			else
				deleteRead(kq, evList);

		}

//with kqueue it will always be size == 1;

	for (std::vector<int>::iterator it = invalidSockets.begin(); it != invalidSockets.end(); ++it)
	{
		std::cout << "close "<< *it << "\n";
		close(*it);
		serverClients.erase(*it);
	}
	invalidSockets.clear();
}


void	Server::changeToWrite(int kq, struct kevent &evList)
{
	struct kevent event;

	// Change the event to EVFILT_WRITE filter
	EV_SET(&event, evList.ident , EVFILT_READ, EV_DELETE, 0, 0, NULL);
	if (kevent(kq, &event, 1, NULL, 0, NULL) == -1) {
		perror("kevent");
		exit(EXIT_FAILURE);
	}
	EV_SET(&event, evList.ident, EVFILT_WRITE, EV_ADD, 0, 0, NULL);
	if (kevent(kq, &event, 1, NULL, 0, NULL) == -1) {
		perror("kevent");
		exit(EXIT_FAILURE);
	}
}

void	Server::changeToRead(int kq, struct kevent &evList)
{
	struct kevent event;

	// Change the event to EVFILT_READ filter
	EV_SET(&event, evList.ident , EVFILT_WRITE, EV_DELETE, 0, 0, NULL);
	if (kevent(kq, &event, 1, NULL, 0, NULL) == -1) {
		perror("kevent");
		exit(EXIT_FAILURE);
	}
	EV_SET(&event, evList.ident, EVFILT_READ, EV_ADD, 0, 0, NULL);
	if (kevent(kq, &event, 1, NULL, 0, NULL) == -1) {
		perror("kevent");
		exit(EXIT_FAILURE);
	}
}

void	Server::deleteWrite(int kq, struct kevent &evList)
{
	struct kevent event;

	EV_SET(&event, evList.ident , EVFILT_WRITE, EV_DELETE, 0, 0, NULL);
	if (kevent(kq, &event, 1, NULL, 0, NULL) == -1) {
		perror("kevent");
		exit(EXIT_FAILURE);
	}
}

void	Server::deleteRead(int kq, struct kevent &evList)
{
	struct kevent event;

	EV_SET(&event, evList.ident , EVFILT_READ, EV_DELETE, 0, 0, NULL);
	if (kevent(kq, &event, 1, NULL, 0, NULL) == -1) {
		perror("kevent");
		exit(EXIT_FAILURE);
	}
}
#endif
