#ifndef SERVER_HPP
#define SERVER_HPP

#include "headers.hpp"
#include "client.hpp"

class server
{
    private:
        int                 fd_server;
        int                 port;
        struct sockaddr_in  addr_server;
        int                 maxfd;
        std::map<int, client>    clients;
        fd_set              read_set;
        fd_set              write_set;
    
        

    public:
        //server();
        //~server(){};

        void    setup();
        void    run();

        void    accept_client();
        void    read_request(int fd);
        void    send_response(int fd);
        //void    set_port(char *)
};




#endif