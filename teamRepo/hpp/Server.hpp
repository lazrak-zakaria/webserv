#ifndef __SERVER__
#define __SERVER__

#include "ServerConfig.hpp"
#include "Client.hpp"
#include "MimeAndError.hpp"
// #define _KQUEUE_

class Server
{
	private:
	
		int										fdSock;
		struct sockaddr_in  					addrServer;
		ServerConfig							*configData;
		std::map<std::string, ServerConfig*>	*serverNamesConfig;
		std::map<int, Client>					serverClients;
		MimeAndError							*mimeError;
		std::vector<int>						invalidSockets;

	public:

		void	socketBindListen();
		int		maxSockClient() const;
		void	setConfigDataServerNames(std::map<std::string, ServerConfig*> *serversName);
		void	setConfigData(ServerConfig	*c);
		void	setMimeError(MimeAndError	*m);
		int		getFdSock();


		#ifndef _KQUEUE_
		void	checkClientsTimeout(fd_set &readSet, fd_set &writeSet);
		void	acceptClient(fd_set &readSet);
		void	processReadySockets(fd_set &tempReadSet, 
						fd_set &tempWriteSet,fd_set &readSet, fd_set &writeSet);
		#else
		void	acceptClient(int kq);
		bool	isClientExist(int fd);
		void	changeToWrite(int kq, struct kevent &evList);
		void	changeToRead(int kq, struct kevent &evList);
		void	deleteWrite(int kq, struct kevent &evList);
		void	deleteRead(int kq, struct kevent &evList);
		void	processReadySockets(int kq, struct kevent& evList);
		#endif


};

#endif