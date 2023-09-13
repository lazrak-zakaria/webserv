#ifndef __SERVER__
#define __SERVER__

#include "server_config.hpp"
#include "client.hpp"
#include "mime_and_status_code.hpp"

class server
{
	private:
		int						fd_sock;
		struct sockaddr_in  	addr_server;
	
		std::map<int, client>	server_clients;
		server_config			*config_data;
		mime_and_status_code	*mime_status_code;
	public:
		void	socket_bind_listen();
		int		return_max_sock_client() const;
		void	accept_client(fd_set &read_set);
		void	handling_ready_sockets(fd_set &temp_read_set, 
						fd_set &temp_write_set,fd_set &read_set, fd_set &write_set);
		
};

#endif