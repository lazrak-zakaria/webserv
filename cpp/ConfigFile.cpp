#include "../hpp/headers.hpp"
#include "../hpp/ServerConfig.hpp"

std::vector<std::string> split(std::string str, char limiter)
{
    std::stringstream ss(str);
    std::string a;
    std::vector<std::string> result;

    while (std::getline(ss, a, limiter))
    {
        result.push_back(a);
    }
    return (result);
}

void printError_exit(std::string strError)
{
    std::cout << strError << std::endl;
    exit(1);
}

size_t parseSizeT(std::string str)
{
    int count = 0;

    for (size_t i = 0; i < str.size(); i++)
    {
        if (!std::isdigit(str[i]))
            printError_exit("Invalid character in LimitBodySize: not a digit");
        count++;
    }
    if (count > 17)
        printError_exit("LimitBodySize exceeds maximum size_t limit");
    std::stringstream ss(str);
    size_t result = 0;
    ss >> result;
    if (result > 18446744073709551)
        printError_exit("LimitBodySize exceeds maximum size_t value");
    return (result);
}

int parseErrorPages(std::string str)
{
    int count = 0;
    int result;

    for (size_t i = 0; i < str.size(); i++, count++)
    {
        if (!std::isdigit(str[i]))
            printError_exit("Invalid character in Listen: not a digit");
    }
    result = std::atoi(str.c_str());
    if (count > 3 || result > 599)
        printError_exit("Listen port out of allowed range");
    return (result);
}

void validateAndCopyPorts(std::vector<std::string> &_split, std::vector<int> &listen)
{
    size_t j;
    int count;

    for (size_t i = 0; i < _split.size(); i++)
    {
        std::string str = _split[i];
        for (count = 0, j = 0; j < str.size(); j++, count++)
        {
            if (!std::isdigit(str[j]))
                printError_exit("Listen is not digit !");
        }
        listen.push_back(std::atoi(str.c_str()));
        if (count > 5 || listen[i] > 65535)
            printError_exit("Maximum port out of rang !");
    }
}

void ServerConfig::parseConfig(std::list<ServerConfig> &allConfigs, std::string configName,
                               std::vector<std::pair<ServerConfig *, std::map<std::string, ServerConfig *> > > &answer)
{
    int                         i;
    std::string                 host_port;
    std::vector<std::string>    _split;
    std::ifstream               inputFile(configName.c_str());
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
    std::map<std::string, std::pair<ServerConfig *, std::map<std::string, ServerConfig *> > > _check;

    if (!inputFile)
    {
        std::cerr << "Error: Failed to open the input file." << std::endl;
        exit(1);
    }
    //                         0      1      2              3             4                  5                    6          7         8        9                           10                 11              12                13            14            15          16     17      18                19
    std::string _server[] = {"[\n", "]\n", "\thost: ", "\tlisten: ", "\terror_page: ", "\tlimitBodySize: ", "\tlocation: ", "\t{\n", "\t}\n", "\t\tallowedMethods: ", "\t\tredirection: ", "\t\troot: ", "\t\tautoIndex: ", "\t\tindex: ", "\t\tcgi: ", "\t\tcanUpload: ", "\n", "#", "\t\talias: ", "\tserverName: "};
    std::string line;
    while (std::getline(inputFile, line))
    {
        line += '\n';
        for (i = 0; i < 20; i++)
        {
            if (!line.compare(0, _server[i].size(), _server[i]))
                break;
        }
        switch (i)
        {
            case 0: // "[\n"
                if (flag_server)
                    printError_exit("Error in config file: Unexpected opening bracket encountered");
                flag_server = 1;
            break;

            case 1: // "]\n"
                if (!flag_server)
                    printError_exit("Error in config file: Unexpected closing bracket without an opening bracket");
                flag_server = 0;
                if (tlisten.empty())
                    printError_exit("Error in config file : Port is empty");
                else if (_newServer.host.empty())
                    printError_exit("Error in config file : Host is empty");
                for (size_t j = 0; j < tlisten.size(); j++)
                {
                    std::stringstream ss;
                    ss << tlisten[j];

                    _newServer.port = tlisten[j];
                    allConfigs.push_back(_newServer);
                    host_port = _newServer.host + ss.str();
                    if (!_check.count(host_port))
                    {
                        _check[host_port];
                        _check[host_port].first = &*(--allConfigs.end());
                    }
                    for (size_t l = 0; l < server_name.size(); l++)
                    {
                        if (_check[host_port].second.count(server_name[l]))
                            printError_exit("Error in config file : Same server name");
                        _check[host_port].second[server_name[l]];
                        _check[host_port].second[server_name[l]] = &*(--allConfigs.end());
                    }
                }
                server_name.clear();
                tlisten.clear();
                _newServer.clear();
                break;

            case 2: // "\thost: "
                line.erase(--line.end());
                if (!flag_server || flag_location || !_newServer.host.empty())
                    printError_exit("Error in config file : host");
                _newServer.host = line.substr(_server[i].size(), line.size());
                break;

            case 3: // "\tlisten: "
                line.erase(--line.end());
                if (!flag_server || flag_location || !tlisten.empty())
                    printError_exit("Error in config file : listen");
                _split = split(line.substr(_server[i].size(), line.size()), ' ');
                if (_split.empty())
                    printError_exit("Error in config file : Listen is empty");
                validateAndCopyPorts(_split, tlisten);
                _split.clear();
                break;

            case 4: // "\terror_page "
                line.erase(--line.end());
                if (!flag_server || flag_location)
                    printError_exit("Error in config file : error_page");
                _split = split(line.substr(_server[i].size(), line.size()), ' ');
                if (_split.size() != 2)
                    printError_exit("Error in config file : Error_page is empty");
                _newServer.errorPages[parseErrorPages(_split[0])] = _split[1];
                _split.clear();
                break;

            case 5: // "\tlimitBodySize: "
                line.erase(--line.end());
                if (!flag_server || flag_location || flag_Size)
                    printError_exit("Error in config file : limitBodySize");
                flag_Size = 1;
                _newServer.limitBodySize = parseSizeT(line.substr(_server[i].size(), line.size()));
                break;

            case 6: // "\tlocation "
                line.erase(--line.end());
                if (!flag_server || flag_location || flag_path)
                    printError_exit("Error in config file : location");
                path_location = line.substr(_server[i].size(), line.size());
                if (path_location.empty())
                    printError_exit("Error in config file : Path_location is empty");
                flag_path = 1;
                break;

            case 7: // "\t{\n"
                if (!flag_server || flag_location || !flag_path)
                    printError_exit("Error in config file: Unexpected curly braces");
                flag_location = 1;
                break;

            case 8: // "\t}\n"
                if (!flag_server || !flag_location)
                    printError_exit("Error in config file: Unexpected closing curly braces");
                flag_location = 0;
                flag_path = 0;
                flag_poort_alias = 0;
                if (_newLocation.root.empty() && _newLocation.alias.empty())
                    printError_exit("Error in config file : Root and Alias is empty");
                if (_newLocation.allowedMethods.empty())
                    _newLocation.allowedMethods.insert("GET");
                _newServer.allLocations[path_location] = _newLocation;
                _newLocation.free_all();
                break;

            case 9: // "\t\tallowedMethods: "
                line.erase(--line.end());
                if (!flag_server || !flag_location || !_newLocation.allowedMethods.empty())
                    printError_exit("Error in config file : allowedMethods");
                _split = split(line.substr(_server[i].size(), line.size()), ' ');
                if (_split.empty())
                    printError_exit("Error in config file : Path_location is empty");
                for (size_t j = 0; j < _split.size(); j++)
                {
                    if (!_newLocation.allowedMethods.insert(_split[j]).second)
                        printError_exit("Error in config file : Methods is duplicate");
                }
                _split.clear();
                break;

            case 10: // "\t\tredirection: "
                line.erase(--line.end());
                if (!flag_server || !flag_location || !_newLocation.redirection.empty())
                    printError_exit("Error in config file : redirection");
                _newLocation.redirection = line.substr(_server[i].size(), line.size());
                if (_newLocation.redirection.empty())
                    printError_exit("Error in config file : Redirection is empty");
                break;

            case 11: // "\t\troot: "
                line.erase(--line.end());
                if (!flag_server || !flag_location || !_newLocation.root.empty() || flag_poort_alias)
                    printError_exit("Error in config file : root");
                flag_poort_alias = 1;
                _newLocation.root = line.substr(_server[i].size(), line.size());
                if (_newLocation.root.empty())
                    printError_exit("Error in config file : Root is empty");
                break;

            case 12: //"\t\tautoIndex: "
                line.erase(--line.end());
                if (!flag_server || !flag_location)
                    printError_exit("Error in config file : autoIndex");
                if (line.substr(_server[i].size(), line.size()) == "yes")
                    _newLocation.autoIndex = 1;
                else if (line.substr(_server[i].size(), line.size()) == "no")
                    _newLocation.autoIndex = 0;
                else
                    printError_exit("Error in config file : AutoIndex (yes) or (no)");
                break;

            case 13: //"\t\tindex: "
                line.erase(--line.end());
                if (!flag_server || !flag_location || !_newLocation.index.empty())
                    printError_exit("error in config file : index");
                _newLocation.index = split(line.substr(_server[i].size(), line.size()), ' ');
                if (_newLocation.index.empty())
                    printError_exit("Error in config file : Index is empty");
                break;

            case 14: // "\t\tcgi: "
                line.erase(--line.end());
                if (!flag_server || !flag_location)
                    printError_exit("Error in config file : cgi");
                _split = split(line.substr(_server[i].size(), line.size()), ' ');
                if (_split.size() != 2)
                    printError_exit("Error in config file : cgi");
                _newLocation.cgi[_split[0]] = _split[1];
                _split.clear();
                break;

            case 15: // "\t\tcanUpload: "
                line.erase(--line.end());
                if (!flag_server || !flag_location)
                    printError_exit("Error in config file : canUpload");
                if (line.substr(_server[i].size(), line.size()) == "yes")
                    _newLocation.canUpload = 1;
                else if (line.substr(_server[i].size(), line.size()) == "no")
                    _newLocation.canUpload = 0;
                else
                    printError_exit("Error in config file : CanUpload (yes) or (no)");
                break;

            case 16: // "\n"
                break;

            case 17: // "#"
                break;

            case 18: // "\t\talias: "
                line.erase(--line.end());
                if (!flag_server || !flag_location || !_newLocation.alias.empty() || flag_poort_alias)
                    printError_exit("Error in config file : alias");
                flag_poort_alias = 1;
                _newLocation.alias = line.substr(_server[i].size(), line.size());
                if (_newLocation.alias.empty())
                    printError_exit("Error in config file : Alias is empty");
                break;

            case 19: // "\tserverName: "
                line.erase(--line.end());
                if (!flag_server || flag_location || !server_name.empty())
                    printError_exit("Error in config file : serverName");
                _split = split(line.substr(_server[i].size(), line.size()), ' ');
                if (_split.empty())
                    printError_exit("Error in config file : ServerName is empty");
                server_name = _split;
                _split.clear();
                break;

            default:
                printError_exit("Error in config file: Unknown configuration entry");
        }
    }
    std::map<std::string, std::pair<ServerConfig *, std::map<std::string, ServerConfig *> > >::iterator it;
    for (it = _check.begin(); it != _check.end(); it++)
    {
        answer.push_back(it->second);
    }
    if (flag_server == 1 || answer.empty())
        printError_exit("Error in config file: The file is empty or missing closing bracket");
    inputFile.close();
}
