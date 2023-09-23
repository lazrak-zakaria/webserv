#ifndef _WEB_SERVER__HPP__
#define _WEB_SERVER__HPP__

#include "./Server.hpp"

class	WebServer
{
	public:
		static void	run(std::map<int, Server*> &serversConfig);

};

#endif
