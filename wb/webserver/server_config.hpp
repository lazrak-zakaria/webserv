#ifndef SERVER_CONFIG_HPP
#define SERVER_CONFIG_HPP

#include "header.hpp"

typedef struct location
{
	std::vector<std::string>			methods;
	std::string							redirection;
	std::string							root;
	std::string							alias;
	bool								directory_listing;
	std::string							default_file_directory;
	std::vector<std::string>			index;
	std::map<std::string, std::string>	cgi; // map< file extension , cgi program> 

	bool								upload_store; //path to store file being uploaded

} location;

class server_config
{
	private:

	public:
		int									port;
		std::string							host;
		std::string							server_name; //vector
		std::vector<std::string>			index;
		std::string							root;
		std::map<std::string, std::string>	error_pages;
		std::map<std::string, location>		all_locations;
		size_t								limit_body_size;

		server_config();
		~server_config();
};



#endif

