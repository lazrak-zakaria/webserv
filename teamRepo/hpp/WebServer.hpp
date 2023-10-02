#ifndef _WEB_SERVER__HPP__
#define _WEB_SERVER__HPP__

#include "./Server.hpp"

class	WebServer
{
		static void initializeSockets(std::vector<std::pair<ServerConfig, std::map<std::string, ServerConfig*> > > &serversConfig,
				std::vector<Server> &serversVec, std::map<int, Server*> &ServersMap, MimeAndError *mime);
	public:
		static void	run(std::vector<std::pair<ServerConfig, std::map<std::string, ServerConfig*> > >&serversConfig);

};

#endif


/*

achraf 
	vector of {deafultConf, std::map<string, conf*}


	vector of server

*/