#include "../hpp/headers.hpp"
#include "../hpp/ServerConfig.hpp"

std::vector<std::string> split(std::string str, char limiter)
{
    std::stringstream ss(str);
    std::string a;
    std::vector<std::string> result;

    while (std::getline(ss, a, limiter)) {
        result.push_back(a);
    }
    return (result);
}

void    printError_exit(std::string strError)
{
    std::cout << strError << std::endl;
    exit (1);
}

size_t  getSise_t(std::string str)
{
    int count = 0;
    for (int i = 0; i < str.size(); i++) {
        if (!std::isdigit(str[i]))
            printError_exit("limitBodySize is not digit !");
        count++;
    }
    if (count > 17)
        printError_exit("limitBodySize = max size_t");
    std::stringstream   ss(str);
    size_t              result = 0;
    ss >> result;
    if (result > 18446744073709551)
        printError_exit("limitBodySize = max size_t");
    return (result);
}

int get_numberError(std::string str) {
    int count = 0;
    int result;

    for (int i = 0; i < str.size(); i++, count++) {
        if (!std::isdigit(str[i]))
            printError_exit("errorPages is not digit !");
    }
    result = std::atoi(str.c_str());
    if (count > 3 || result > 599)
        printError_exit("errorPages is Greater than allowed !");
    return (result);
}

void    checkPorts_andCopy(std::vector<std::string> &_split, std::vector<int> &listen)
{
    int j;
    int count;
    for (int i = 0; i < _split.size(); i++) {
        std::string str = _split[i];
        for (count = 0, j = 0; j < str.size(); j++, count++) {
            if (!std::isdigit(str[j]))
                printError_exit("listen is not digit !");
        }
        listen.push_back(std::atoi(str.c_str()));
        if (count > 5 || listen[i] > 65535)
            printError_exit("maximum port out of rang !");
    }
}

void    ServerConfig::parseConfig(std::list<ServerConfig> &allConfigs, std::string configName,
	std::vector<std::pair<ServerConfig*, std::map<std::string, ServerConfig*> > > &answer)
{
    int                         i;
    std::string                 host_port;
    std::vector<std::string>    _split;
    std::ifstream               inputFile(configName);
    int                         flag_location = 0;
    std::string                 path_location;
    int                         flag_path = 0;
    int                         flag_server = 0;
    int                         flag_Size = 0;
    int                         flag_poort_alias = 0;
    std::vector<int>            tlisten;
    std::vector<std::string>    server_name;
    ServerConfig                _newServer;
    location                    _newLocation;
    std::map<std::string, std::pair<ServerConfig*, std::map<std::string, ServerConfig*> > > _check;
    if (!inputFile) {
        std::cerr << "Error: Failed to open the input file." << std::endl;
        exit (1);
    }
    //                         0      1      2              3             4                  5                    6          7         8        9                           10                 11              12                13            14            15          16     17      18                19        
    std::string _server[] = {"[\n", "]\n", "\thost: ", "\tlisten: ", "\terror_page ", "\tlimitBodySize: ", "\tlocation: ", "\t{\n", "\t}\n", "\t\tallowedMethods: ", "\t\tredirection: ", "\t\troot: ", "\t\tautoIndex: ", "\t\tindex: ", "\t\tcgi: ", "\t\tcanUpload: ", "\n", "#", "\t\talias: ", "\tserverName: "};
    std::string line;
    while(std::getline(inputFile, line)) {
        line += '\n';
        for (i = 0; i < 20; i++) {
            if (!line.compare(0, _server[i].size(), _server[i]))
                break ;
        }
        switch (i) {
            case 0:  // "[\n"
                if (flag_server)
                    printError_exit("error in config file : case 0");
                flag_server = 1;
                break ;
            case 1:  // "]\n"
                if (!flag_server)
                    printError_exit("error in config file : case 1");
                flag_server = 0;
                if (tlisten.empty() || _newServer.host.empty())
                    printError_exit("error in config file : case 1 : port || host is empty");
                for (int j = 0; j < tlisten.size(); j++) {
                    std::stringstream ss;
                    ss << tlisten[j];

                    _newServer.port = tlisten[j];
                    allConfigs.push_back(_newServer);
                    host_port = _newServer.host + ss.str();
                    if (!_check.count(host_port)) {
                        _check[host_port];
                        _check[host_port].first = & *(--allConfigs.end());
                    }
                    for (int l = 0; l < server_name.size(); l++) {
                        if (_check[host_port].second.count(server_name[l]))
                            printError_exit("error in config file : same server name");
                        _check[host_port].second[server_name[l]];
                        _check[host_port].second[server_name[l]] = & *(--allConfigs.end());
                    }
                }
                server_name.clear();
                tlisten.clear();
                _newServer.clear();
                break ;
            case 2:  // "\thost: "
                line.pop_back();
                if (!flag_server || flag_location || !_newServer.host.empty())
                    printError_exit("error in config file : case 2");
                _newServer.host = line.substr(_server[i].size(), line.size());
                break ;
            case 3: // "\tlisten: "
                line.pop_back();
                if (!flag_server || flag_location || !tlisten.empty())
                    printError_exit("error in config file : case 3");
                _split = split (line.substr(_server[i].size(), line.size()), ' ');
                if (_split.empty())
                    printError_exit("error in config file : case 3 : tlisten");
                checkPorts_andCopy(_split, tlisten);
                _split.clear();
                break ;
            case 4: // "\terror_page "
                line.pop_back();
                if (!flag_server || flag_location)
                    printError_exit("error in config file : case 4");
                _split = split(line.substr(_server[i].size(), line.size()), ' ');
                if (_split.size() != 2)
                    printError_exit("error in config file : case 4 : terror_page");
                _newServer.errorPages[get_numberError(_split[0])] = _split[1];
                _split.clear();
                break ;
            case 5:  // "\tlimitBodySize: "
                line.pop_back();
                if (!flag_server || flag_location || flag_Size)
                    printError_exit("error in config file : case 5");
                flag_Size = 1;
                _newServer.limitBodySize = getSise_t(line.substr(_server[i].size(), line.size()));
                break ;
            case 6:  // "\tlocation "
                line.pop_back();
                if (!flag_server || flag_location || flag_path)
                    printError_exit("error in config file : case 6");
                path_location = line.substr(_server[i].size(), line.size());
                if (path_location.empty())
                    printError_exit("error in config file : path_location is emty");
                flag_path = 1;
                break ;
            case 7:  // "\t{\n"
                if (!flag_server || flag_location || !flag_path)
                    printError_exit("error in config file : case 7");
                flag_location = 1;
                break ;
            case 8:  // "\t}\n"
                if (!flag_server || !flag_location)
                    printError_exit("error in config file : case 8");
                flag_location = 0;
                flag_path = 0;
                flag_poort_alias = 0;
                if (_newLocation.allowedMethods.empty())
                    _newLocation.allowedMethods.insert("GET");
                _newServer.allLocations[path_location] = _newLocation;
                _newLocation.free_all();
                break ;
            case 9:   // "\t\tallowedMethods: "
                line.pop_back();
                if (!flag_server || !flag_location || !_newLocation.allowedMethods.empty())
                    printError_exit("error in config file : case 9"); 
                _split = split(line.substr(_server[i].size(), line.size()), ' ');
                if (_split.empty())
                    printError_exit("error in config file : path_location is emty");
                for (int j = 0; j < _split.size(); j++) {
                    if (!_newLocation.allowedMethods.insert(_split[j]).second)
                        printError_exit("error in config file : set<>");
                }
                _split.clear();
                break ;
            case 10:  // "\t\tredirection: "
                line.pop_back();
                if (!flag_server || !flag_location || !_newLocation.redirection.empty())
                    printError_exit("error in config file : case 10");
                _newLocation.redirection = line.substr(_server[i].size(), line.size());
                if (_newLocation.redirection.empty())
                    printError_exit("error in config file : redirection is emty");
                break ;
            case 11:  // "\t\troot: "
                line.pop_back();
                if (!flag_server || !flag_location || !_newLocation.root.empty() || flag_poort_alias)
                    printError_exit("error in config file : case 11");
                flag_poort_alias = 1;
                _newLocation.root = line.substr(_server[i].size(), line.size());
                if (_newLocation.root.empty())
                    printError_exit("error in config file : root is emty");
                break ;
            case 12:  //"\t\tautoIndex: "
                line.pop_back();
                if (!flag_server || !flag_location)
                    printError_exit("error in config file : case 12");
                if (line.substr(_server[i].size(), line.size()) == "yes")
                    _newLocation.autoIndex = 1;
                else if (line.substr(_server[i].size(), line.size()) == "no")
                    _newLocation.autoIndex = 0;
                else
                    printError_exit("error in config file : case 12 : else (yes, no)");
                break ;
            case 13:  //"\t\tindex: "
                line.pop_back();
                if (!flag_server || !flag_location || !_newLocation.index.empty())
                    printError_exit("error in config file : case 13");
                _newLocation.index = split(line.substr(_server[i].size(), line.size()), ' ');
                if (_newLocation.index.empty())
                    printError_exit("error in config file : index is emty");
                break ;
            case 14: // "\t\tcgi: "
                line.pop_back();
                if (!flag_server || !flag_location)
                    printError_exit("error in config file : case 14");
                _split = split(line.substr(_server[i].size(), line.size()), ' ');
                if (_split.size() != 2)
                    printError_exit("error in config file : cgi");
                _newLocation.cgi[_split[0]] = _split[1];
                _split.clear();
                break ;
            case 15:  // "\t\tcanUpload: "
                line.pop_back();
                if (!flag_server || !flag_location)
                    printError_exit("error in config file : case 15");
                if (line.substr(_server[i].size(), line.size()) == "yes")
                    _newLocation.canUpload = 1;
                else if (line.substr(_server[i].size(), line.size()) == "no")
                    _newLocation.canUpload = 0;
                else
                    printError_exit("error in config file : case 15 : else (no, yes)");
                break ;
            case 16: // "\n"
                break ;
            case 17: // "#"
                break ;
            case 18: // "\t\talias: "
                line.pop_back();
                if (!flag_server || !flag_location || !_newLocation.alias.empty() || flag_poort_alias)
                    printError_exit("error in config file : case 18");
                flag_poort_alias = 1;
                _newLocation.alias = line.substr(_server[i].size(), line.size());
                if (_newLocation.alias.empty())
                    printError_exit("error in config file : alias is emty");
                break ;
            case 19:  // "\tserverName: "
                line.pop_back();
                if (!flag_server || flag_location || !server_name.empty())
                    printError_exit("error in config file : case 19");
                _split = split(line.substr(_server[i].size(), line.size()), ' ');
                if (_split.empty())
                    printError_exit("error in config file : case 19 : empty");
                server_name = _split;
                _split.clear();
                break ;
            default:
                printError_exit("error in config file : default");
            }
        }
        std::map<std::string, std::pair<ServerConfig*, std::map<std::string, ServerConfig*> > >::iterator it;
        for (it = _check.begin();it != _check.end(); it++) {
            answer.push_back(it->second);
    }
    inputFile.close();
}
