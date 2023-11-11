#include "../hpp/headers.hpp"
#include "../hpp/Client.hpp"
#include "../hpp/Server.hpp"
#include "../hpp/WebServer.hpp"


int main(int ac, char **argv)
{
	if (ac > 2)
	{
		std::cerr << "accept only one config file\n";
		exit(0);
	}
	
	std::list<ServerConfig> allConfigs;
	std::vector<std::pair<ServerConfig*, std::map<std::string, ServerConfig*> > > serverConfigs;

	ServerConfig::parseConfig(allConfigs, argv[1] ? argv[1] : "./configFiles/default.conf", serverConfigs);
	WebServer::run(serverConfigs);

	exit(0);
}
