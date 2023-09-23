#include "../hpp/headers.hpp"
#include "../hpp/Client.hpp"
#include "../hpp/Server.hpp"
#include "../hpp/WebServer.hpp"


void	setLocation(std::map<int, Server*> &mp, Server &s, 	MimeAndError &mime);


int main()
{
	Server s;
	MimeAndError mime;
	std::map<int, Server*> mp;

	setLocation(mp, s, mime);


	WebServer::run(mp);

	return 0;
}
