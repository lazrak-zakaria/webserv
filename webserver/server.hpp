#ifndef __SERVER__
#define __SERVER__

#include "server_config.hpp"
#include "client.hpp"

class server
{
	private:
		int						fd_sock;
		struct sockaddr_in  	addr_server;
	
		std::map<int, client>	server_clients;
		server_config			*config_data;

	public:
		void	socket_bind_listen();
		int		return_max_sock_client() const;
		void	accept_client(fd_set &read_set);
		void	handling_ready_sockets(fd_set &temp_read_set, 
						fd_set &temp_write_set,fd_set &read_set, fd_set &write_set);
		
};

#endif