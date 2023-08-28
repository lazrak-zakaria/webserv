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
  /*
  
  
  #include <iostream>
#include <string>

int main() {
    std::string request = "POST /example HTTP/1.1\r\n"
                          "Host: www.example.com\r\n"
                          "Transfer-Encoding: chunked\r\n"
                          "\r\n"
                          "7\r\n"
                          "Chunk 1\r\n"
                          "6\r\n"
                          "Chunk 2\r\n"
                          "0\r\n"
                          "\r\n";

    std::string body;
    bool is_chunked = false;
    size_t chunk_size = 0;

    // Parse the request header to determine if the request body is chunked
    size_t pos = request.find("\r\n\r\n");
    if (pos != std::string::npos) {
        std::string header = request.substr(0, pos);
        if (header.find("Transfer-Encoding: chunked") != std::string::npos) {
            is_chunked = true;
        }
    }

    // Parse the request body if it is chunked
    if (is_chunked) {
        pos = request.find("\r\n\r\n") + 4;
        while (pos < request.size()) {
            size_t chunk_pos = request.find("\r\n", pos);
            if (chunk_pos == std::string::npos) {
                break;
            }
            chunk_size = std::stoi(request.substr(pos, chunk_pos - pos), nullptr, 16);
            if (chunk_size == 0) {
                break;
            }
            pos = chunk_pos + 2;
            body += request.substr(pos, chunk_size);
            pos += chunk_size + 2;
        }
    }

    std::cout << "Request body: " << body << std::endl;

    return 0;
}
*/

//more cheking later
void    request::parse_body_chunked()
{
    size_t  body_size = body.size();
    
    size_t      chunk_pos = body.find("\r\n"); // chnukpos != ::npos;
    if (chunk_pos == std::string::npos)
        return ;
    std::string chunk_size_string = body.substr(0, chunk_pos);
    size_t  chunk_size ;
    while (chunk_pos < body_size)
    {
        // std::stringstream ss;
        // ss << std::hex << chunk_size_string;
        // ss >> chunk_size;
        chunk_size = strtol(chunk_size_string.c_str(), NULL, 16);
        if (!chunk_size)
            break;
        body_parsed += body.substr(chunk_pos + 2, chunk_size) + "\n";
        size_t start = chunk_pos + 2 + chunk_size + 2;
        chunk_pos = body.find("\r\n", start);
        chunk_size_string = body.substr(start, chunk_pos - start);
    }
}
