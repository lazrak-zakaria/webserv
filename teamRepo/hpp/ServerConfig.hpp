#ifndef SERVER_CONFIG_HPP
#define SERVER_CONFIG_HPP

#include "./headers.hpp"

typedef struct location
{
	std::set<std::string>				allowedMethods;
	std::string							redirection;
	std::string							root;
	std::string							alias;
	bool								autoIndex;
	std::vector<std::string>			index;
	// map< file extension , cgi program> 
	std::map<std::string, std::string>	cgi;
	bool								canUpload;

	location() {
		canUpload = true;
		autoIndex = true;
	}
	void	free_all() {
		allowedMethods.clear();
		redirection = "";
		root = "";
		alias = "";
		autoIndex = 0;
		index.clear();
		cgi.clear();
		canUpload = 0;
	}

} location;

class ServerConfig
{
	private:

	public:
		int									port;
		std::string							host;
		std::map<int, std::string>			errorPages;
		std::map<std::string, location>		allLocations;
		size_t								limitBodySize;

		ServerConfig() {
			port = 0;
			limitBodySize = 10000000000;
		}
		void	clear() {
			port = 0;
			host = "";
			errorPages.clear();
			allLocations.clear();
			limitBodySize = 10000000000;
		}
		static void parseConfig(std::list<ServerConfig> &allConfigs, std::string configName, 
		std::vector<std::pair<ServerConfig*, std::map<std::string, ServerConfig*> > > &answer);
};

#endif
