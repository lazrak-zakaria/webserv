#ifndef __SERVER__HPP__
#define __SERVER__HPP__

#include "./server.hpp"

class	webserver
{
	public:
		static void	run(std::map<int, server> &servers_config);

};

#endif
