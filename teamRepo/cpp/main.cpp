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

	Client c;
	c.setConfigData(&s);
	c.setMimeError(&mime);


	std::string as = "POST / HTTP/1.1\r\n\
host: value\r\n\
ssss: \tvvvvvvvvvvvvvvvv\r\n\
content-type: multipart/form-data; boundary=abcde12345\r\n\
content-length: 1000\r\n\r\n\
--abcde12345\r\n\
Content-Disposition: form-data; name=\"profileImage\"\r\n\r\n\
blabla\r\n\
--abcde12345\r\n\
Content-Disposition: form-data; name=\"profileImage\"; filename=\"image1.png\"\r\n\
Content-Type: image/png\r\n\r\n\
FILE_CONTENT0\n\r\n\
--abcde12345\r\n\
Content-Disposition: form-data; name=\"profileImage\"; filename=\"image1.png\"\r\n\
Content-Type: image/png\r\n\r\n\
FILE_CONTENT1\n\
";

	std::string b = "FILE_CONTENT2\n\
FILE_CONTENT3\n\
\r\n\
--abcde12345\r\n\
Content-Disposition: form-data; name=\"profileImage\"; filename=\"image1.png\"\r\n\
Content-Type: image/png\r\n\r\n\
qqqqqqqqqqqqqqqqqqqq\r\n\
--abcde12345--\r\n";
	c.readRequest(as.c_str(), as.size());
	c.readRequest(b.c_str(), b.size());
	return 0;
}
