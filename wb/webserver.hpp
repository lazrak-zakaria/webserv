#ifndef __SERVER__HPP__
#define __SERVER__HPP__

#include "server.hpp"

class	webserver
{
	public:
		void				run(std::map<int, server> &servers_config);
		fd_set              read_set;
        fd_set              write_set;
};

#endif
