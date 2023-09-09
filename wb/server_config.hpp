#ifndef CONF__
#define CONF__

#include <string>
#include <vector>
#include <map>

//https://docs.nginx.com/nginx/admin-guide/web-server/serving-static-content/

// https://stackoverflow.com/questions/1694144/can-two-applications-listen-to-the-same-port   suprise
// https://www.quora.com/Can-two-applications-listen-to-the-same-port


// server_name
	//https://serverfault.com/questions/1062957/is-server-name-really-needed-for-default-server-nginx


//https://www.reddit.com/r/AskNetsec/comments/wzk23o/what_is_server_name_in_nginx/?rdt=46513


typedef struct location
{
	std::vector<std::string>	methods;
	std::string					redirection;
	std::string					root;
	std::string					alias;
	bool						directory_listing;
	std::string					default_file_directory;

	std::map<std::string, std::string> cgi; // map< file extension , cgi program> 

	bool					upload_store; //path to store file being uploaded

} location;

class server_config
{
	private:

	public:
		std::string					port;
		std::string					host;
		std::string					server_name;

		std::vector<std::string>	index;
		std::string	root; // It applies to all location {} blocks where the root directive is not included to explicitly redefine the root:

		size_t		limit_body_size;
		std::map<std::string, std::string>	error_pages;
		
		std::map<std::string, location>		all_locations;

		

		int								fd_sock;
		void							socket_bind_listen();

		server_config();
		~server_config();
};



#endif

/*
	if a location --> cgi != empty ---> as cgi
*/
