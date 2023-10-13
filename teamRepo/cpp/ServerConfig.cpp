#include "./../hpp/ServerConfig.hpp"


location::location()
{
	canUpload = true;
	autoIndex = true;
}

ServerConfig::ServerConfig()
{
	port = 0;
	limitBodySize = 1000000000;
}