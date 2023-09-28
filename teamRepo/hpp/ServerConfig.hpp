#ifndef SERVER_CONFIG_HPP
#define SERVER_CONFIG_HPP

#include "./headers.hpp"

typedef struct location
{
	std::set<std::string>			allowedMethods;
	std::string							redirection;
	std::string							root;
	std::string							alias;
	bool								autoIndex;
	std::vector<std::string>			index;
	// map< file extension , cgi program> 
	std::map<std::string, std::string>	cgi;
	bool								canUpload;

} location;

class ServerConfig
{
	private:

	public:
		int									port;
		std::string							host;
		std::string							serverNames; //vector
		std::vector<std::string>			index;
		// std::string						root;
		std::map<u_int8_t, std::string>		errorPages;
		std::map<std::string, location>		allLocations;
		size_t								limitBodySize;

};

#endif
