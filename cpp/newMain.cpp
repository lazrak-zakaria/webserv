#include "../hpp/headers.hpp"
#include "../hpp/Client.hpp"
#include "../hpp/Server.hpp"
#include "../hpp/WebServer.hpp"


void print_server(std::list<ServerConfig> &servers) {

	std::list<ServerConfig>::iterator it = servers.begin();
	for (int i = 1;it != servers.end(); it++, i++)
	{
		std::cout << "===============server[" << i << "]===============\n";
		std::cout << "port = (" << it->port << ")\n";
		std::cout << "host = (" << it->host << ")\n";
		for (std::map<int, std::string>::iterator it1 = it->errorPages.begin(); it1 != it->errorPages.end(); it1++) {
			std::cout << "error_page = (" << it1->first << ") (" << it1->second << ")\n";
		}
		std::cout << "limitBodySize = (" << it->limitBodySize << ")\n";
		std::map<std::string, location>::iterator it2 = it->allLocations.begin();
		for (int j = 1; it2 != it->allLocations.end(); it2++, j++) {
			std::cout << "------------location[" << j << "]------------\n";
			std::cout << "path_location = (" << it2->first << ")\n";
			std::cout << "allowedMethods = ";
			for (std::set<std::string>::iterator it3 = it2->second.allowedMethods.begin(); it3 != it2->second.allowedMethods.end(); it3++)
				std::cout << "(" << *it3 << ") ";
			std::cout << '\n';
			std::cout << "redirection = (" << it2->second.redirection << ")\n";
			std::cout << "root = (" << it2->second.root << ")\n";
			std::cout << "alias = (" << it2->second.alias << ")\n";
			std::cout << "autoIndex = (" << it2->second.autoIndex << ")\n";
			std::cout << "allowedMethods = ";
			for (int c = 0; c < it2->second.index.size(); c++)
				std::cout << "(" << it2->second.index[c] << ") ";
			std::cout << "\n";
			for (std::map<std::string, std::string>::iterator it4 = it2->second.cgi.begin(); it4 != it2->second.cgi.end(); it4++) {
				std::cout << "cgi: (" << it4->first << ") (" << it4->second << ")\n";
			}
			std::cout << "canUpload = (" << it2->second.canUpload << ")\n";
		}
	}
}

int main(int ac, char **argv)
{
	if (ac > 2)
	{
		std::cerr << "accept only one config file\n";
		exit(0);
	}
	
	std::list<ServerConfig> allConfigs;
	std::vector<std::pair<ServerConfig*, std::map<std::string, ServerConfig*> > > serverConfigs;
	ServerConfig::parseConfig(allConfigs, argv[1] ? argv[1] : "./default.config", serverConfigs);

	// print_server(allConfigs);
	WebServer::run(serverConfigs);
	exit(0);

	// for (auto it = serverConfigs.begin(); it != serverConfigs.end(); ++it)
	// {
	// 	if (it->second.empty())
	// 		printServer(it->first);
	// 	else
	// 	{
	// 		for (auto itt = it->second.begin(); itt != it->second.end(); ++itt)
	// 		{
	// 			std::cerr << "server name: " << itt->first << "\n";
	// 			printServer(itt->second);
	// 		}
	// 	}
	// }
	
}
