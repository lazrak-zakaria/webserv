#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include "header.hpp"
#include "server_config.hpp"
#include "request.hpp"

class response
{
    public:
        int			code;	
        std::string	response_line;
		std::string date;
		std::string server;
		std::string connection;
		std::string content_type;
		std::string last_modified;
    	std::string body;
		size_t		content_length;


		std::string		header;
		std::ifstream	ifs;
		bool			flag_open;
		std::string		key_location;
		std::string		final_path; // path li knt3amal maah

		void			detect_final_location(server_config& server, request& http_request);
		void			serve_response(server_config& server, request& http_request);
		void			get_method(server_config& server, request& http_request);
		void			post_method(server_config& server, request& http_request);
		void			delete_method(server_config& server, request& http_request);
		bool	start_with(const std::string &location_directive, const std::string &path);
};

#endif

/*
RESPONSE 	depend on the path and the

post request 
*/

