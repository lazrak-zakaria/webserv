#ifndef SERVER_CONFIG_HPP
#define SERVER_CONFIG_HPP

#include "header.hpp"

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
		
		server_config();
		~server_config();
};



#endif

