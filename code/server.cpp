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
    port = 8059;
    addr_server.sin_family = AF_INET;
    addr_server.sin_addr.s_addr = INADDR_ANY; // ?? i just copied this from tutorials
    addr_server.sin_port = htons(port);
    if (bind(fd_server, (struct sockaddr *) &addr_server,
              sizeof(addr_server)) < 0) 
    {
        perror("bind");
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
        if (select(mx + 1, &r, &w, 0, 0) == -1)
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
                    // std::cout << "reeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee\n";
                    read_request((*it).first);
                    FD_CLR((*it).first, &r);
                    FD_SET((*it).first, &w);
                }
                else if (FD_ISSET((*it).first, &w))
                {
                    //build res and send
                    // std::cout << "weeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee\n";
                
                    send_response((*it).first);
                    FD_CLR((*it).first, &w);
                    FD_SET((*it).first, &r);
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
    std::cout << "++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n";
    int r = recv(fd, clients[fd].client_request + clients[fd].received,  , 0);
    if (r == -1 )
    {
        std::cerr << "connetion closed\n";
        return ;
    }
    // if (r == REQ_SIZE)
    // {
    // }
    clients[fd].received += r;
    clients[fd].client_request[clients[fd].received] = '\0';
    char *q = strstr(clients[fd].client_request, "\r\n\r\n");
    if (!q && !clients[fd].start_body){
        return ;}
    else
        clients[fd].start_body = 1;
    // std::cout << clients[fd].client_request << "\n";
    FD_CLR(fd, &read_set);
    // FD_CLR(fd, &r);
    FD_SET(fd, &write_set);
    clients[fd].http_request.parse_header(clients[fd].client_request);//
    bzero(clients[fd].client_request, sizeof((clients[fd].client_request)));
    clients[fd].received = 0;
}


void    server::send_response(int fd)
{
    std::string s = "HTTP/1.1 200 OK\r\nConnection: keep-alive\r\nContent-Length: 773\r\n\
Content-Type: text/html\r\n\r\n";
    int f = open("./index.html", O_RDONLY);
    char buf[41000];
    int r = read(f, buf, 41000);
    buf[r] = 0;
    //  std::cout << "--------------------------------------------------------------> " << r << "\n";
    send(fd, s.c_str(), s.length(), 0);
    send(fd, buf, r, 0);
    //std::cout << "here\n";
    FD_CLR(fd, &write_set);
    FD_SET(fd, &read_set);
    //close (f);
}

