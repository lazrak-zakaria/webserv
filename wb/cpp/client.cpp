#include "../client.hpp"


client::client()
{
	memset(&client_sockaddr, 0, sizeof(client_sockaddr));
	client_sockaddr_length = 0;
}
client::~client()
{
	
}