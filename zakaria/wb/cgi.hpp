#ifndef	__CGI_HPP__
#define __CGI_HPP__

#include "./header.hpp"
#include "./server.hpp"
#include "./request.hpp"

class cgi
{
	public:
		// environement variables;
		std::string	REQUEST_METHOD;
		std::string	QUERY_STRING;
		std::string	CONTENT_LENGTH;
		std::string	CONTENT_TYPE;

		// std::string	HTTP_COOKIE;
		std::vector<std::string> env_variables;
		void	execute_cgi(server & server_conf, request& http_request);

};

#endif