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
		

		MimeAndError			*mimeError;
		std::vector<int>		invalidSockets;
	public:
		std::map<int, Client>	serverClients;
		void	socketBindListen();
		int		maxSockClient() const;
		void	acceptClient(int kq);
		void	processReadySockets(int kq, struct kevent& evList);

		void	setConfigDataServerNames(std::map<std::string, ServerConfig*> *serversName);
		void	setConfigData(ServerConfig	*c);
		void	setMimeError(MimeAndError	*m);
		int		getFdSock();
		void	checkClientsTimeout(fd_set &readSet, fd_set &writeSet);


		void	changeToWrite(int kq, struct kevent &evList);
		void	changeToRead(int kq, struct kevent &evList);

		void	deleteWrite(int kq, struct kevent &evList);
		void	deleteRead(int kq, struct kevent &evList);

};

#endif