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
	
		std::map<int, Client>	serverClients;
		ServerConfig			*configData;
		MimeAndError			*mimeError;
		std::set<int>			invalidSockets;				
	public:
		void	socketBindListen();
		int		maxSockClient() const;
		void	acceptClient(fd_set &readSet);
		void	processReadySockets(fd_set &tempReadSet, 
						fd_set &tempWriteSet,fd_set &readSet, fd_set &writeSet);


						
		void	setConfigData(ServerConfig	*c);
		void	setMimeError(MimeAndError	*m);
		int		getFdSock();
};

#endif