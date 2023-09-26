/* temporary file */

#include "../hpp/headers.hpp"
#include "../hpp/Server.hpp"


void	setLocation(std::map<int, Server*> &mp, ServerConfig	&server1, 	MimeAndError &mime)
{
	server1.host = "127.0.0.1";
	server1.port = 9090;
	server1.serverNames = "serv";
	server1.limitBodySize = 10000000000;

	std::vector<location> loc(2);

	loc[0].alias = "/nfs/homes/zlazrak/Desktop/ww/teamRepo/test/";
	loc[0].index.push_back("indexo.html");
	loc[0].index.push_back("index.html");
	loc[0].index.push_back("smile.png");
	loc[0].allowedMethods.push_back("GET");
	loc[0].allowedMethods.push_back("POST");
	loc[0].canUpload = 1;
	loc[1].alias = "/nfs/homes/zlazrak/Desktop/ww/team/testFiles/";
	loc[1].allowedMethods.push_back("POST");
	loc[1].canUpload = true;



	server1.allLocations["/"] = loc[0];
	server1.allLocations["/Upload/"] = loc[1];

}

/*

/upload/
{
	alias "/nfs/homes/zlazrak/Desktop/ww/team/testFiles" 
}

*/