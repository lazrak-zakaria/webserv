#include "server.hpp"


// todo : optimise strstr to not always search from first
void    server::read_request(int fd)
{
    bool    move_to_write = false;
    int r = recv(fd, clients[fd].client_request + clients[fd].received, REQ_SIZE - clients[fd].received, 0);
    if (r == -1)
    {
        //close & remove client
        exit(0);
    }
    clients[fd].received += r;
    clients[fd].client_request[clients[fd].received] = 0;
    if (!clients[fd].start_body)
    {
        if (clients[fd].received == REQ_SIZE)
        {
            // length exceeded;
        }
        char *is_header_complete = strstr(clients[fd].client_request, "\r\n\r\n");
        if (is_header_complete && !clients[fd].start_body)
        {
            *is_header_complete = 0;
            clients[fd].http_request.parse_header(clients[fd].client_request);
            if (clients[fd].http_request.headers.count("transfer-encoding")
                || clients[fd].http_request.headers.count("content-length"))
            {
                memmove(clients[fd].client_request, is_header_complete + 4, strlen(is_header_complete + 4));
                clients[fd].start_body = true;
                clients[fd].first_found_body = true;

            }
            else move_to_write = true;
        }
        return ;
    }
    else    
    {
        clients[fd].http_request.body += clients[fd].client_request;
        if (clients[fd].http_request.headers.count("transfer-encoding"))
        {
            int ch = clients[fd].http_request.headers["transfer-encoding"].size() - 1;
            if (clients[fd].http_request.headers["transfer-encoding"][ch] == "chunked")
            {
                int last_chunk = clients[fd].http_request.body.find("\r\n0");
                if (last_chunk != std::string::npos)
                {
                    // found final
                    // parse_chunked
                    move_to_write = true;
                }
            }
        }
        else if (clients[fd].http_request.headers.count("content-length"))
        {
            if (atoi(clients[fd].http_request.headers["content-length"][1].c_str()) == clients[fd].http_request.body.size())
            {
                // parse_body
                move_to_write = true;
            }
        }
        clients[fd].received = 0;
    }

    if (move_to_write)
    {
        FD_CLR(fd, &read_set);
        FD_SET(fd, &write_set);
    }
}
