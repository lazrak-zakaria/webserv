#ifndef CLIENT_HPP__
#define CLIENT_HPP__

// class client
// {
//     public: //later private

//     int                 fd_client;
//     char                client_request[REQ_SIZE];
//     int                 received;
//     struct sockaddr_in  addr_client; // accept func
//     socklen_t           addr_length; // accept func

//     request             http_request;

//     bool                start_body;
//     bool                first_found_body;
//     std::string         body;


  
// };

// #include <cstdlib>
#include "./header.hpp"
class server;
#include "./request.hpp"
// #include "./response.hpp"

class client
{
	private:

	public:
		int					fd_sock;
		// char				*client_request;
		size_t				received;
		struct sockaddr_in	client_sockaddr;
		socklen_t			client_sockaddr_length;

		request				http_request;
		// response			http_response;


		client(/* args */);
		client(const client&);
		client& operator=(const client&);
		~client();
};


#endif