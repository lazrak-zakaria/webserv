#include "headers.hpp"


#include "request.hpp"

class client
{
    public: //later private

    int                 fd_client;
    char                client_request[REQ_SIZE];
    int                 received;
    struct sockaddr_in  addr_client; // accept func
    socklen_t           addr_length; // accept func

    request             http_request;
};
