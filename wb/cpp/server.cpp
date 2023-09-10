#include "../server.hpp"


void	server::socket_bind_listen()
{
	fd_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (fd_sock == -1)
    {
        perror("socket");
        exit(1);
    }
    bzero(&addr_server, sizeof(addr_server));
    port = 8069; // later will depend on config file
    addr_server.sin_family = AF_INET;
    addr_server.sin_addr.s_addr = INADDR_ANY; // ?? i just copied this from tutorials
    addr_server.sin_port = htons(port);
    if (bind(fd_sock, (struct sockaddr *) &addr_server,
              sizeof(addr_server)) < 0) 
    {
        perror("bind");
        exit(1);
    }
    listen(fd_sock, 10); // change the 10 later;
}

int	server::return_max_sock_client() const
{
	if (server_clients.empty()) return -1;
	std::map<int, client>::const_iterator	it;
	it = server_clients.end();
	--it;
	return (it->first);
}

void	server::accept_client(fd_set &read_set)
{

	struct sockaddr_in  client_sockaddr;
	socklen_t			client_sockaddr_length ;
	int	fd_sock_tmp;
	memset(&client_sockaddr, 0, sizeof(client_sockaddr));
	client_sockaddr_length = 0;
	fd_sock_tmp = accept(fd_sock, (struct sockaddr*)&client_sockaddr,  &client_sockaddr_length);
	if (fd_sock_tmp == -1)
	{
		std::cerr << "connetion error\n";
        return ;
	}
	FD_SET(fd_sock_tmp, &read_set);
	server_clients[fd_sock_tmp].fd_sock = fd_sock_tmp;
}

void	server::handling_ready_sockets(fd_set &temp_read_set, fd_set &temp_write_set,fd_set &read_set, fd_set &write_set)
{
	std::map<int , client>::iterator it;
	for (it = server_clients.begin(); it != server_clients.end(); ++it)
	{
		// std::cout << "DEBUG\n";
		const int &client_fd_sock = it->first;
		client	&client_obj = it->second;
		if (FD_ISSET(client_fd_sock, &temp_read_set))
		{
			std::cout << "DEBUG\n";
			char	buf[10000];
			int		collected = recv(client_fd_sock, buf, 9999, 0);
			if (collected == -1)
			{
				//should drop client
				exit(0); // just testing
			}
			buf[collected] = '\0';
			std::cout << buf << "\n";
			client_obj.http_request.parse(buf);
			// if (client_obj.http_request.finished_flag)
			// {
				FD_CLR(client_fd_sock, &read_set);
				FD_SET(client_fd_sock, &write_set);
			// }
		}
		else if (FD_ISSET(client_fd_sock, &temp_write_set))
		{
			const std::string &answer = client_obj.http_response.answer;
			int		sent = send(client_fd_sock, answer.c_str(), answer.size(), 0);
			if (answer.size() != sent)
			{
				std::cerr << "client close connection\n";
				exit(0);
				// testing
				// drop client;
			}
			if (client_obj.http_response.finished_flag)
			{
				FD_CLR(client_fd_sock, &write_set);
				FD_SET(client_fd_sock, &read_set);
			}
		}
	}
}


/*
multiple servers


*/