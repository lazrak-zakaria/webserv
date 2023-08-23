#include "server.hpp"


void    server::setup()
{
    fd_server = socket(AF_INET, SOCK_STREAM, 0);
    if (fd_server == -1)
    {
        perror("socket");
        exit(1);
    }
    bzero(&addr_server, sizeof(addr_server));
    port = 8086;
    addr_server.sin_family = AF_INET;
    addr_server.sin_addr.s_addr = INADDR_ANY; // ?? i just copied this from tutorials
    addr_server.sin_port = htons(port);
    if (bind(fd_server, (struct sockaddr *) &addr_server,
              sizeof(addr_server)) < 0) 
    {
        perror("socket");
        exit(1);
    }
    listen(fd_server, 10);
}

void    server::run()
{
    setup();
    FD_ZERO(&read_set);
    FD_ZERO(&write_set);
    FD_SET(fd_server, &read_set);
    while (1)
    {
        int mx = std::max(fd_server, clients.size() ? (*(--clients.end())).first: fd_server);
        fd_set  r = read_set, w = write_set;
        if (select(mx + 1, &r, &w, 0, 0))
        {
            perror("select");
            exit(1);
        }
        if (FD_ISSET(fd_server, &r))
        {
            //call accept
            accept_client();
        }
        else
        {
            for (std::map<int, client>::iterator it = clients.begin(); it != clients.end(); ++it)
            {

                if (FD_ISSET((*it).first, &r))
                {
                    //read req and parse it;
                    read_request((*it).first);
                }
                else if (FD_ISSET((*it).first, &write_set))
                {
                    //build res and send
                }
            }
        }
    }
}









/****************************/

void    server::accept_client()
{
    client  __client;
    bzero(&__client, sizeof(client));
    bzero(&__client.addr_client, sizeof(__client.addr_client));
    __client.fd_client = accept(fd_server, (struct sockaddr*)&(__client.addr_client), &__client.addr_length);
    if (__client.fd_client == -1)
    {
        std::cerr << "connetion error\n";
        return ;
    }
    int fd = __client.fd_client;
    FD_SET(__client.fd_client, &read_set);
    clients[__client.fd_client] = __client;
    std::cout << "new connection in " << fd << " " << clients.size() << "\n";
}


void    server::read_request(int fd)
{
    int r = recv(fd, clients[fd].client_request + clients[fd].received, REQ_SIZE - clients[fd].received, 0);
    if (r == -1)
    {
        std::cerr << "connetion closed\n";
        return ;
    }
    // if (r == REQ_SIZE)
    // {
    // }
    clients[fd].received += r;
    clients[fd].client_request[clients[fd].received] = '\0';
    std::cout << clients[fd].client_request << "\n";

    clients[fd].http_request.parse(clients[fd].client_request);//
}

