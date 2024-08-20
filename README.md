# HTTP-WebServer
> An HTTP/1.1 Webserver built from scratch using C++.

## Table of Contents
* [Overview](#overview)
* [Features](#features)
* [Usage](#usage)


## Overview
- This web server is fully compliant with HTTP/1.1, developed using C++98 and the Standard Template Library (STL).
- It efficiently handles multiple simultaneous connections and requests without relying on multithreading. The server leverages I/O multiplexing to manage all tasks within a single thread.
- Networking is built using the UNIX socket API, ensuring robust communication with connected clients.

## Features
- GET, POST, DELETE methods
- Can be configured with a configuration file
- CGI (Common Gateway Interface) to execute dynamic scripts such as PHP, Python, Perl,..., enabling server-side scripting and     dynamic content generation.
- Cookies and session management
- Multiple websites
- Autoindex (directory listing)
- Direct uploads


## Usage
in terminal, clone:
```
git clone https://github.com/lazrak-zakaria/webserv.git && cd webserv
```
run:
```
make
```
run the web server:
```bash
./webserv cgi-bin/default.config
```
Now open a browser and enter the IP address and port from the configuration file, followed by a valid URL that corresponds to a valid location.[configuration file](cgi-bin/default.config)

 
 ```
 ip-address:port/URL

 127.0.0.2:3030
 ```

