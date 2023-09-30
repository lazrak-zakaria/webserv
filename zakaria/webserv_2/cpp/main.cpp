#include "../client.hpp"
#include "../mime_and_status_code.hpp"
#include "../server.hpp"
#include "../webserver.hpp"


// https://stackoverflow.com/questions/76412255/how-to-fix-error-141-when-using-select-and-send-in-c-web-server-for-multip
	// exit code 141

int main(int ac, char **av)
{
	server_config	server1;
	mime_and_status_code	mime;
	server1.host = "127.0.0.1";
	server1.port = atoi(av[1]);
	server1.server_name = "z";
	server1.limit_body_size = 10000000000;
	server1.root = "/nfs/homes/zlazrak/Desktop/wbs/webfiles";

	std::vector<location> loc(4);

	loc[0].alias = "/nfs/homes/zlazrak/Desktop/ww/zakaria/webfiles";
	loc[0].index.push_back("indexo.html");
	loc[0].index.push_back("index.html");
	loc[0].index.push_back("smile.png");
	loc[0].methods.push_back("GET");
	loc[0].methods.push_back("POST");

	loc[1].root = "/nfs/homes/zlazrak/Desktop/wbs/webfiles";
	loc[1].methods.push_back("GET"); // turn methode to map later;
	loc[1].index.push_back("myimage");
	
	loc[2].root = "/nfs/homes/zlazrak/Desktop/wbs/webserver";
	loc[2].methods.push_back("POST"); // turn methode to map later;
	loc[2].methods.push_back("DELETE"); // turn methode to map later;
	loc[2].upload_store = true;
	// loc[2].cgi[".py"] = "/usr/bin/python3";

	loc[3].root = "/nfs/homes/zlazrak/Desktop/wbs/webfiles";
	loc[3].methods.push_back("POST"); // turn methode to map later;
	loc[3].methods.push_back("DELETE"); // turn methode to map later;

	loc[3].cgi[".py"] = "/usr/bin/python3";
	loc[3].cgi[".php"] = "/usr/bin/php";
	loc[3].cgi[".sh"] = "/usr/bin/sh";

	server1.all_locations["/"] = loc[0];
	server1.all_locations["/images/"] = loc[1];
	server1.all_locations["/Uploadaewaewa"] = loc[2];
	server1.all_locations["/cgi-bin"] = loc[3];


	// client clienta;
	// clienta.set_config_data(&server1);
	// clienta.set_mime_status_code(&mime);
	// std::ifstream ifs("../request.txt");

	server	sv;
	sv.set_mime_status_code(&mime);
	sv.set_config_data(&server1);
	sv.socket_bind_listen();
	std::map<int, server*> sec;
	sec[sv.get_fd_sock()] = &sv;
	webserver::run(sec);
}
