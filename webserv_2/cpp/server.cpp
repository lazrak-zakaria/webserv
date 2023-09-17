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
    addr_server.sin_family = AF_INET;
	// https://stackoverflow.com/questions/16508685/understanding-inaddr-any-for-socket-programming
    addr_server.sin_addr.s_addr = INADDR_ANY;
    addr_server.sin_port = htons(config_data->port);
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
	server_clients[fd_sock_tmp];
	server_clients[fd_sock_tmp].set_config_data(config_data);
	server_clients[fd_sock_tmp].set_mime_status_code(mime_status_code);
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
			// if (!collected)
			// 	FD_CLR(client_fd_sock, &read_set);
			if (collected == -1)
			{
				server_clients.erase(it->first);
				FD_CLR(client_fd_sock, &read_set);
				std::cout << "drop client\n";

				//should drop client
				// exit(0); // just testing
			}
			buf[collected] = '\0';
			// std::cout << buf << "\n";
			std::string	tmp;
			int i = 0;
			while (i < collected)
				tmp.push_back(buf[i++]);
			client_obj.serve_client(tmp, collected);
			if (client_obj.is_request_finished())
			{
				FD_CLR(client_fd_sock, &read_set);
				FD_SET(client_fd_sock, &write_set);
			}
		}
		else if (FD_ISSET(client_fd_sock, &temp_write_set))
		{
			std::cout << "SSSEE\n";
			std::string answer = client_obj.serve_client("", 0);
			if(client_obj.is_response_finished())
				answer += "\r\n";
			// std::cout << answer << "\n";
			int		sent = send(client_fd_sock, answer.c_str(), answer.size(), 0);
			
			if (answer.size() != sent)
			{
				std::cerr << "client close connection\n";
				exit(0);
				// testing
				// drop client;
			}
			if (client_obj.is_response_finished())
			{
				// exit(2);
				client_obj.clear();
				FD_CLR(client_fd_sock, &write_set);
				FD_SET(client_fd_sock, &read_set);
			}
		}
	}
}

void	server::set_config_data(server_config	*c)
{
	config_data = c;
}

void	server::set_mime_status_code(mime_and_status_code	*m)
{
	mime_status_code = m;
}

int		server::get_fd_sock()
{
	return fd_sock;
}


/*
multiple servers


*/