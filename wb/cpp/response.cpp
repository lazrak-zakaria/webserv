#include "../response.hpp"

bool	response::start_with(const std::string &location_directive,const  std::string &path)
{
	return (path.substr(0, location_directive.length()) == location_directive);
}

void	response::detect_final_location(server& server_conf, request& http_request)
{
	std::map<std::string, location>::iterator	it;
	std::vector<std::string> location_set;
	for (it = server_conf.all_locations.begin(); it == server_conf.all_locations.end(); ++it)
	{
		if (start_with(it->first, http_request.uri.path))
			location_set.push_back(it->first);
	}
	if (location_set.empty())
		return ;
	size_t	most_descriptive_location = location_set[0].length();
	size_t	location_set_size = location_set.size();
	size_t	index_of_most_descriptive_location = 0;
	for (size_t i = 1; i < location_set_size; ++i)
	{
		size_t	len = location_set[i].length();
		if (len > most_descriptive_location)
		{
			most_descriptive_location = len;
			index_of_most_descriptive_location = i;
		}
	}
	this->key_location = location_set[index_of_most_descriptive_location];
	if (!server_conf.all_locations[key_location].root.empty()) // not empty
	{
		std::string &tmp = server_conf.all_locations[key_location].root;
		this->final_path = tmp + http_request.uri.path;
	}
	else
	{
		std::string &tmp = server.all_locations[key_location].alias;
		this->final_path = tmp + http_request.uri.path.substr(tmp.length());
	}
}

void	response::serve_response(server& server_conf, request& http_request)
{
	if (final_path.empty())
	{
		detect_final_location(server_conf, http_request);
	}	
	if (http_request.uri.method == "GET")
	{

	}
	else if (http_request.uri.method == "POST")
	{
		
	}
	else if (http_request.uri.method == "DELETE")
	{
		
	}
}



void	response::get_method(server& server_conf, request& http_request)
{
	if (!body_sending)
	{
		struct stat	path_stat;
		if (S_ISREG(path_stat.st_mode))
		{
			//cgi later;
			ifs.open(final_path);
			if (!ifs.is_open())
			{
				//permision denied
			}
			//	build header;
			//	after sending header turn on body_sending;
		}
		else if (S_ISDIR(path_stat.st_mode))
		{
			if (server_conf.all_locations[key_location].index.size())
			{

			}
			else if (server_conf.all_locations[key_location].index.directory_listing)
			{
				
			}
		}
	}
	else
	{
		
	}
}

/*
because im 
*/
