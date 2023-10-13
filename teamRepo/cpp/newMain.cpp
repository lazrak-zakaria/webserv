#include "../hpp/headers.hpp"
#include "../hpp/Client.hpp"
#include "../hpp/Server.hpp"
#include "../hpp/WebServer.hpp"
#include "../hpp/ConfigFile.hpp"



// void printServer(ServerConfig* ser)
// {
// 	ServerConfig & s = *ser;

// 	std::cerr << "--------------------------------------------------\n";
// 	std::cerr << s.host << " : " << s.port << "\n";
// 	std::cerr << "limit: " << s.limitBodySize << "\n";
// 	for (auto it = s.errorPages.begin(); it != s.errorPages.end(); ++it)
// 		std::cerr << it->first << " " << it->second << "\n";
	
// 	for (auto it = s.allLocations.begin(); it != s.allLocations.end(); ++it)
// 	{
// 		std::cerr << it->first << "\n" ;
// 		std::cerr << "\t\talias " << it->second.alias << "\n";
// 		std::cerr << "\t\tautoIndex " << it->second.autoIndex << "\n";
// 		std::cerr << "\t\tcanUpload " << it->second.canUpload << "\n";
// 		std::cerr << "\t\tredirection " << it->second.redirection << "\n";
// 		std::cerr << "\t\troot " << it->second.root << "\n";

// 		for (auto itt = it->second.allowedMethods.begin(); itt != it->second.allowedMethods.end(); ++itt)
// 			std::cerr << "\t\t" << *itt << "\n";
		
// 		for (auto itt = it->second.index.begin(); itt != it->second.index.end(); ++itt)
// 				std::cerr << "\t\t" << *itt << "\n";

// 		for (auto itt = it->second.cgi.begin(); itt != it->second.cgi.end(); ++itt)
// 				std::cerr << "\t\t" << itt->first << " " << itt->second << "\n";
// 	}
// 	std::cerr << "--------------------------------------------------\n";
// }

int main(int ac, char **argv)
{
	if (ac > 2)
	{
		std::cerr << "accept only one config file\n";
		exit(0); 
	}
	
	std::list<ServerConfig> allConfigs;
	std::vector<std::pair<ServerConfig*, std::map<std::string, ServerConfig*> > > serverConfigs;
	ConfigFile::parseConfig(allConfigs, argv[1] ? argv[1] : "./../default.config", serverConfigs);
	
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
