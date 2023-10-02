#ifndef __SERVER__
#define __SERVER__

#include "ServerConfig.hpp"
#include "Client.hpp"
#include "MimeAndError.hpp"

class Server
{
	private:
		int						fdSock;
		struct sockaddr_in  	addrServer;
	
		ServerConfig							*configData;
		std::map<std::string, ServerConfig*>	*serverNamesConfig;
		
		std::map<int, Client>	serverClients;

		MimeAndError			*mimeError;
		std::vector<int>		invalidSockets;
	public:
		void	socketBindListen();
		int		maxSockClient() const;
		void	acceptClient(fd_set &readSet);
		void	processReadySockets(fd_set &tempReadSet, 
						fd_set &tempWriteSet,fd_set &readSet, fd_set &writeSet);

		void	setConfigDataServerNames(std::map<std::string, ServerConfig*> *serversName);
		void	setConfigData(ServerConfig	*c);
		void	setMimeError(MimeAndError	*m);
		int		getFdSock();
};

#endif