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
#include <dirent.h>

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <stack>
#include <deque>
#include <algorithm>
#include <sstream>
#include <fstream>

#define SIZE_READ 8


#endif
/*
								cgi ? run
				can open
								serve
reg    :      
				cant open		error


				autoindex		serve or error
dir		:	
				index files		try all of them until one is opened

				error
*/