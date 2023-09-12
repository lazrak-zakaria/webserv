#include "../client.hpp"


client::client()
{
	memset(&client_sockaddr, 0, sizeof(client_sockaddr));
	client_sockaddr_length = 0;
}
client::~client()
{
	
}



/*




client
{
	have request and response
	what does the client need from server to handle request?
		- configs
}






*/