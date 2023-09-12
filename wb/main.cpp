#include "header.hpp"

#include "./webserver.hpp"
#include "./server.hpp"

int	main()
{
	server	server1;
	/********************manualy right now*********************/
	
	server1.host = "127.0.0.1";
	server1.port = 8090;
	server1.server_name = "z";

	server1.root = "/nfs/homes/zlazrak/Desktop/wbs/webfiles";

	std::vector<location> loc(4);

	loc[0].alias = "/nfs/homes/zlazrak/Desktop/wbs/webfiles";
	loc[0].index.push_back("index.html");
	loc[0].methods.push_back("GET");
	loc[0].methods.push_back("POST");

	loc[1].root = "/nfs/homes/zlazrak/Desktop/wbs/webfiles";
	loc[1].methods.push_back("GET"); // turn methode to map later;
	
	loc[2].root = "/nfs/homes/zlazrak/Desktop/wbs/webfiles";
	loc[2].methods.push_back("POST"); // turn methode to map later;
	loc[2].methods.push_back("DELETE"); // turn methode to map later;


	loc[3].root = "/nfs/homes/zlazrak/Desktop/wbs/webfiles";
	loc[3].methods.push_back("POST"); // turn methode to map later;
	loc[3].methods.push_back("DELETE"); // turn methode to map later;

	loc[3].cgi[".py"] = "/usr/bin/python3";
	loc[3].cgi[".php"] = "/usr/bin/php";
	loc[3].cgi[".sh"] = "/usr/bin/sh";

	server1.all_locations["/"] = loc[0];
	server1.all_locations["/images/"] = loc[1];
	server1.all_locations["/upload"] = loc[2];
	server1.all_locations["/cgi-bin"] = loc[3];


	/**********************************************************/
	// server1.socket_bind_listen(); // later i will do this on webserver::run
	// std::map<int, server> servers_config;
	// servers_config[server1.fd_sock] = server1;
	
	// webserver::run(servers_config);


	return 0;
}
