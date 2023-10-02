#include "../hpp/headers.hpp"
#include "../hpp/Client.hpp"
#include "../hpp/Server.hpp"
#include "../hpp/WebServer.hpp"


void	setLocation(std::map<int, Server*> &mp, ServerConfig	&server1, 	MimeAndError &mime);


int main()
{
	ServerConfig s;
	MimeAndError mime;
	std::map<int, Server*> mp;

	setLocation(mp, s, mime);

	std::vector<std::pair<ServerConfig, std::map<std::string, ServerConfig*> > > sser;

	std::pair<ServerConfig, std::map<std::string, ServerConfig*> > pr;

	pr.first = s;
	std::vector<std::pair<ServerConfig, std::map<std::string, ServerConfig*> > > v;
	v.push_back(pr);
	WebServer::run(v);

	// Client c;
	// c.setDefaultConfigData(&s);
	// c.setMimeError(&mime);


// 	std::string as = "POST / HTTP/1.1\r\n\
// host: value\r\n\
// ssss: \tvvvvvvvvvvvvvvvv\r\n\
// content-type: multipart/form-data; boundary=abcde12345\r\n\
// content-length: 1000\r\n\r\n\
// --abcde12345\r\n\
// Content-Disposition: form-data; name=\"profileImage\"\r\n\r\n\
// blabla\r\n\
// --abcde12345\r\n\
// Content-Disposition: form-data; name=\"profileImage\"; filename=\"image1.png\"\r\n\
// Content-Type: image/png\r\n\r\n\
// FILE_CONTENT0\n\r\n\
// --abcde12345\r\n\
// Content-Disposition: form-data; name=\"profileImage\"; filename=\"image1.png\"\r\n\
// Content-Type: image/png\r\n\r\n\
// FILE_CONTENT1\r\n\
// --abcde12345--\r\n\
// ";

// 	std::string b = "FILE_CONTENT2\n\
// FILE_CONTENT3\n\
// \r\n\
// --abcde12345\r\n\
// Content-Disposition: form-data; name=\"profileImage\"; filename=\"image1.png\"\r\n\
// Content-Type: image/png\r\n\r\n\
// qqqqqqqqqqqqqqqqqqqq\r\n\
// --abcde12345--\r\n";


// std::string as = "GET / HTTP/1.1\r\n\
// host: value\r\n\
// ssss: \tvvvvvvvvvvvvvvvv\r\n\
// connection: close\r\n\r\n";


// // connection: close\r\n\r\n";

// 	std::ofstream ff("../output/outt.txt");

// 	while (true)
// 	{
// 		if (c.isRequestFinished() == false)
// 			c.readRequest(as.c_str(), as.size());
// 		else if (c.isResponseFinished() == false)
// 		{
// 			std::string &res = c.serveResponse();
// 			std::cout << res << std::flush;
// 			// std::cout << res << res.size()<<  "\n";
// 			// ff.write(res.c_str(), res.size());
// 		}
// 		// else
// 		// {
// 		// 	break;
// 		// }
// 	}
	// c.readRequest(as.c_str(), as.size());

	// std::cout << c.serveResponse() << "\n";
	// std::cout << c.serveResponse() << "\n";
	// std::cout << c.serveResponse() << "\n";
	// std::cout << c.serveResponse() << "\n";
	// std::cout << c.serveResponse() << "\n";
	// std::cout << c.serveResponse() << "\n";
	// c.readRequest(b.c_str(), b.size());
	return 0;
}
