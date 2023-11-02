#include "../hpp/ServerConfig.hpp"

location::location() {
	canUpload = true;
	autoIndex = true;
}

void    location::free_all() {
	allowedMethods.clear();
	redirection = "";
	root = "";
	alias = "";
	autoIndex = 0;
	index.clear();
	cgi.clear();
	canUpload = 0;
}

ServerConfig::ServerConfig() {
	port = 0;
	limitBodySize = 10000000000;
}

void    ServerConfig::clear() {
    port = 0;
    host = "";
    errorPages.clear();
    allLocations.clear();
    limitBodySize = 10000000000;
}