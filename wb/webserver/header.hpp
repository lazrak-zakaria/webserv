#ifndef HEAD__
#define HEAD__

#include <cstdio>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/types.h>
#include <netdb.h>
#include <errno.h>
#include <sys/stat.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <deque>
#include <algorithm>
#include <sstream>
#include <fstream>

#define SIZE_READ 2048

typedef struct location
{
	std::vector<std::string>	methods;
	std::string					redirection;
	std::string					root;
	std::string					alias;
	bool						directory_listing;
	std::string					default_file_directory;
	std::vector<std::string>	index;
	std::map<std::string, std::string> cgi; // map< file extension , cgi program> 

	bool					upload_store; //path to store file being uploaded

} location;

#endif
