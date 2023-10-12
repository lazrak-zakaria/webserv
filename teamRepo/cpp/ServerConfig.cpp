#include "/Users/zlazrak/Desktop/combining/teamRepo/hpp/ServerConfig.hpp"


location::location()
{
	allowedMethods.insert("GET");
	canUpload = true;
	autoIndex = true;
}

ServerConfig::ServerConfig()
{
	port = 0;
	limitBodySize = 1000000000;
}