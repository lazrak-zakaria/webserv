#ifndef __SERVER__HPP__
#define __SERVER__HPP__

#include "server_config.hpp"

class	webserver
{
	public:

		void	run(std::vector<server_config&> &servers_config);

};

#endif
