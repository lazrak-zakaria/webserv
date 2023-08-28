#ifndef HH
#define HH

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

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <unordered_set>
#include <unordered_map>
#include <deque>
#include <algorithm>
#include <sstream>

#define CRLF "\r\n"
#define REQ_SIZE 9000
#define RE 40

#include "server.hpp"

#endif