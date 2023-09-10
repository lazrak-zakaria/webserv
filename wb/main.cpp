#include "header.hpp"

#include "webserver.hpp"
#include "server.hpp"

int	main()
{
	server	server1;
	server1.socket_bind_listen();
	std::map<int, server> servers_config;
	servers_config[server1.fd_sock] = server1;

	webserver web;
	web.run(servers_config);


	return 0;
}
