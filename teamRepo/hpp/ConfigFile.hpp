#include "ServerConfig.hpp"
#include "headers.hpp"

class ConfigFile
{
	private:

	public:
	static void parseConfig(std::list<ServerConfig> &c, std::string configName, 
									std::vector<std::pair<ServerConfig*, std::map<std::string, ServerConfig*> > >	&answer);
};
