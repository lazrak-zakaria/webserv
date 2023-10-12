#include "../hpp/ConfigFile.hpp"


#define LISTEN 0
#define HOST 1
#define REDIRECT 2



void split (const std::string &s, char delim, std::vector<std::string> & vec)
{
    std::stringstream ss (s);
    std::string item;
    while (getline (ss, item, delim)) {
        vec.push_back (item);
    }
}


void	printError(std::string s)
{
	std::cerr << s << "\n";
	exit(1);
}



void ConfigFile::parseConfig(std::list<ServerConfig> &c, std::string configName,
								std::vector<std::pair<ServerConfig*, std::map<std::string, ServerConfig*> > >	&answer)
{
	std::set<std::string> ports;
	std::map<std::string, std::pair<ServerConfig*, std::map<std::string, ServerConfig*> > > hostPort;

	std::string	error;
	std::ifstream ifs(configName.c_str());
	std::string	line;
	bool	done = false;
	int		serversCounter;
	bool	insideServerBlock = false;
	bool	insideLocationBlock = false;
	bool	config[3] = {};
	int		indexConfig = 0;
	size_t	portsSize = 0;

	std::vector<std::string> port;
	std::vector<std::string> host;
	std::vector<std::string> serverNames;
	ServerConfig sconf;
	std::string				locationKey;

	std::map<std::string, std::set<std::string> > hostServerNames;
	int start = 0, end = 0;

	bool	locationOn  = false; // == '{'->true;
	while (getline(ifs, line, '\n'))
	{
		std::cout << line << "\n";
		if (line.empty())
			continue;

		if (insideServerBlock)
		{

			if (line == "[")
			{
				printError("close first server block first");

			}
			else if (line == "]")
			{
				if (!config[0] || !config[1])
				{
					printError("provide a host ip and port");

				}
				else
				{

					// do the job;
					insideServerBlock = false;

					for (size_t i = 1; i < port.size(); ++i)
					{
						std::string newPort = host[1] + port[i];
						sconf.host = host[1];
						sconf.port = atoi(port[i].c_str());
						c.push_back(sconf);
						if (!hostPort.count(newPort))
						{
							hostPort[newPort].first = &  *(--c.end());
							for (size_t j = 1; j < serverNames.size(); ++j)
							{
								hostPort[newPort].second[serverNames[j]] = hostPort[newPort].first;
							}
						}
						else
						{
							for (size_t j = 1; j < serverNames.size(); ++j)
							{
								if (hostPort[newPort].second.count(serverNames[j]))
									printError("same host with same port with same servename ====> ERROR");
								hostPort[newPort].second[serverNames[j]] = &  *(--c.end());
												
							}
						}
					}
					insideLocationBlock = false;
					insideServerBlock = false;
					serverNames.clear();
					port.clear();
					host.clear();
					ServerConfig sssss;//()
					sconf = sssss;
					locationKey = "";
					// memset(config, 0, sizeof(config));
					continue;
				}
			}

			if (insideLocationBlock)
			{
				if (line == "\t{")
					locationOn = true;
				else if (line == "\t}")
				{
					if (locationOn)
					{
						insideLocationBlock = false;
						locationOn = false;
					}
					else
					{
						printError("open location first");
					}
				}
				else if (!line.compare(0, 15, "\t\tredirection: "))
				{
					std::vector<std::string> redir;
					split(line, ' ', redir);
					if (redir.size() != 2)
						printError("location directive redirection error");

					sconf.allLocations[locationKey].redirection = redir[1];
				}
				else if (!line.compare(0, 18, "\t\tallowedMethods: "))
				{
					std::vector<std::string> methodes;
					split(line, ' ', methodes);
					if (methodes.size() < 2)
						printError("location directive methodes error");
					for (size_t i = 0; i < methodes.size(); ++i)
						sconf.allLocations[locationKey].allowedMethods.insert(methodes[i]);
				}
				else if (!line.compare(0, 8, "\t\troot: ") || !line.compare(0, 9, "\t\talias: "))
				{
					std::vector<std::string> rootAlias;
					split(line, ' ', rootAlias);
					if (rootAlias.size() != 2)
						printError("location directive methodes error");
					if (rootAlias[0][2] == 'r')
						sconf.allLocations[locationKey].root = rootAlias[1];
					else
						sconf.allLocations[locationKey].alias = rootAlias[1];
				}
				else if (!line.compare(0, 13, "\t\tcanUpload: "))
				{
					std::vector<std::string> upload;
					split(line, ' ', upload);
					if (upload.size() != 2 || (upload[1] != "yes" && upload[1] != "no"))
						printError("location directive upload error");
					sconf.allLocations[locationKey].canUpload = upload[1][0] == 'y';
				}
				else if (!line.compare(0, 13, "\t\tautoIndex: "))
				{
					std::vector<std::string> autoindex;
					split(line, ' ', autoindex);
					if (autoindex.size() != 2 || (autoindex[1] != "yes" && autoindex[1] != "no"))
						printError("location directive autoindex error");
					sconf.allLocations[locationKey].autoIndex = autoindex[1][0] == 'y';
				}
				else if (!line.compare(0, 7, "\t\tcgi: "))
				{
					std::vector<std::string> cgi;
					split(line, ' ', cgi);
					if (cgi.size() != 3)
						printError("location directive cgi error");
					sconf.allLocations[locationKey].cgi[cgi[1]] = cgi[2];
				}
				else if (!line.compare(0, 9, "\t\tindex: "))
				{
					split(line, ' ', sconf.allLocations[locationKey].index);
					if (sconf.allLocations[locationKey].index.size() < 2)
						printError("location directive index error");
					sconf.allLocations[locationKey].index.erase(sconf.allLocations[locationKey].index.begin());
				}
				else
				{
					std::cout << line << "<\n";
					printError("what !?");
				}
				continue;
			}

			if (!line.compare(0, 9, "\tlisten: "))
			{
				split(line, ' ', port);
				if (port.size() < 2)
					printError("port directive error");
				config[1] = true;
			}
			else if (!line.compare(0, 7, "\thost: "))
			{
				split(line, ' ', host);
				if (host.size() != 2)
					printError("host directive error");
					config[0] = true;
			}
			else if (!line.compare(0, 13, "\tserverName: "))
			{
				split(line, ' ', serverNames);
				if (serverNames.size() < 2)
					printError("servername directive error");
			}
			else if (!line.compare(0, 12, "\terrorPage: "))
			{
				
			}
			else if (!line.compare(0, 11, "\tlocation: "))
			{
				std::vector<std::string> loc;
				split(line, ' ', loc);
				if (loc.size() != 2)
					printError("location directive error");
				insideLocationBlock = true;

				if (sconf.allLocations.count(loc[1]))
					printError("location directive duplicate error");
				sconf.allLocations[loc[1]];
				locationKey = loc[1];
			}
			else if (!line.compare(0, 16, "\tlimitBodySize: "))
			{

			}
			else 
			{
				std::cout << line << '\n';
				printError("what is this !?");
			}
			continue;
		}


		if (line == "[")
		{
			insideServerBlock = true;
		}
		else
		{
			printError("didn't find the entry to the server block");
		}
	}

	if (insideLocationBlock || insideServerBlock)
		printError("please complete the block");

	std::map<std::string, std::pair<ServerConfig*, std::map<std::string, ServerConfig*> > >::iterator it;
	// std::cout <<"{" << c.size() << ";" << c[c.size()-1].host <<"<<\n";	
	for (it  = hostPort.begin(); it != hostPort.end(); ++it)
		answer.push_back(it->second);

}
