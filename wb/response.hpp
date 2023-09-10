#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include "header.hpp"
#include "server.hpp"
#include "request.hpp"

class response
{
    public:
        int			code;	
        std::string	response_line;
		std::string date;
		std::string server_name;
		std::string connection;
		std::string content_type;
		std::string last_modified;
    	std::string body;
		size_t		content_length;

		std::map<std::string, std::string> mime;

		std::string		header;
		std::ifstream	ifs;
		bool			flag_open;
		std::string		key_location;
		std::string		final_path; // path li knt3amal maah

		size_t			received_from_file;
		void			detect_final_location(server& s, request& http_request);
		void			serve_response(server& s, request& http_request);
		void			get_method(server& s, request& http_request);
		void			post_method(server& s, request& http_request);
		// void			delete_method(server& s, request& http_request);
		bool	start_with(const std::string &location_directive, const std::string &path);
		void			create_header();
		std::string		getmime(std::string path);
		std::string	answer;
		bool	body_sending;

		bool	done;
	
		bool	finished_flag;
		response();
};

#endif

/*
RESPONSE 	depend on the path and the

post request 
*/

