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
	std::map<std::string, std::string>	cgi;
	bool								canUpload;

	location();
	void	free_all();

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

		ServerConfig();
		void	clear();
		static void parseConfig(std::list<ServerConfig> &allConfigs, std::string configName, 
		std::vector<std::pair<ServerConfig*, std::map<std::string, ServerConfig*> > > &answer);
};

#endif
