#include "../client.hpp"
#include "../mime_and_status_code.hpp"

int main()
{
	server_config	server1;
	mime_and_status_code	mime;
	server1.host = "127.0.0.1";
	server1.port = 8090;
	server1.server_name = "z";
	server1.limit_body_size = 10000000000;
	server1.root = "/nfs/homes/zlazrak/Desktop/wbs/webfiles";

	std::vector<location> loc(4);

	loc[0].alias = "/nfs/homes/zlazrak/Desktop/wbs/webfiles";
	loc[0].index.push_back("index.html");
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
	server1.all_locations["/Upload"] = loc[2];
	server1.all_locations["/cgi-bin"] = loc[3];


	client clienta;
	clienta.set_config_data(&server1);
	clienta.set_mime_status_code(&mime);
	std::ifstream ifs("../request.txt");

	while (1)
	{
		std::string	tmp ;
		char buf[20];
		ifs.read(buf, 19);
		buf[ifs.gcount()] = '\0'; // mandatory;
		if (!clienta.is_request_finished())
		{
			clienta.serve_client(buf, ifs.gcount());
			// std::cout << "here\n";
		}
		else if (!clienta.is_response_finished())
		{

			// sleep(1);
			std::cout << clienta.serve_client("", 0);
			//break;
		}
		else break;
	}
}
