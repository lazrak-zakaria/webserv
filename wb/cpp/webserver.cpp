#include "../webserver.hpp"

void	webserver::run(std::map<int, server> &servers_config)
{
	FD_ZERO(&read_set);
    FD_ZERO(&write_set);
	std::map<int, server>::iterator it_server;
	for (it_server = servers_config.begin(); it_server != servers_config.end(); ++it_server)
    {
		const int	&fd_server = it_server->first;
		FD_SET(fd_server, &read_set);
	}
	
	while (true)
	{
		it_server = --(servers_config.end());
		int	max_fd = it_server->first;
		for (it_server = servers_config.begin(); it_server != servers_config.end(); ++it_server)
		{
			max_fd = std::max(max_fd, it_server->second.return_max_sock_client());
		}
		fd_set temp_read_set = read_set;
		fd_set temp_write_set = write_set;
		if (select(max_fd + 1, &temp_read_set, &temp_write_set, 0, 0) == -1)
		{
			perror("Select failed");
			exit(1);
		}
		for (it_server = servers_config.begin(); it_server != servers_config.end(); ++it_server)
		{
			const int &fd_server = it_server->first;
			if (FD_ISSET(fd_server, &temp_read_set))
			{
				it_server->second.accept_client(read_set);
			}
		}
		/* i dont know which better do it here or nested with accept*/
		for (it_server = servers_config.begin(); it_server != servers_config.end(); ++it_server)
		{
			it_server->second.handling_ready_sockets(temp_read_set, temp_write_set, read_set, write_set);
		}
	}

}
