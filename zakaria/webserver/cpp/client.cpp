#include "../client.hpp"


client::client()
{
	config_data = NULL;
	memset(&flags, 0, sizeof(flags));
	flags.is_header_ok = true;
	code_status = 0;
	request.me = this;
	response.me = this;
	cgi.me = this;
}

void	client::clear()
{
	memset(&flags, 0, sizeof(flags));
	flags.is_header_ok = true;
	code_status = 0;
	request.req_clear();
	response.res_clear();
	cgi.cgi_clear();

}
client::client(const client & from)
{
	(void)from;
}

client & client::operator = (const client &from)
{
	(void)from;
	return (*this);
}
client::~client()
{}

void				client::set_mime_status_code(mime_and_status_code *m)
{
	mime_status_code = m; 
}

void	client::set_config_data(server_config *config)
{
	config_data = config;
}

bool	client::is_response_finished(void) const
{
	return flags.response_finished;
}

bool	client::is_request_finished(void) const
{
	return	flags.request_finished;
}


bool		client::start_with(const std::string &location_directive, const std::string &path)
{
	size_t	location_length = location_directive.length();
	return (path.substr(0, location_length) == location_directive); // more checks
}

void		client::detect_final_location(void)
{
	std::map<std::string, location>::iterator	it;
	std::vector<std::string> location_set;
	for (it = config_data->all_locations.begin(); it != config_data->all_locations.end(); ++it)
	{
		if (start_with(it->first, request.path))
			location_set.push_back(it->first);
	}
	
	if (location_set.empty())
		return ;// error no match
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
	location_key = location_set[index_of_most_descriptive_location];
	
	if (!config_data->all_locations[location_key].root.empty()) // not empty
	{
		std::string &tmp = config_data->all_locations[location_key].root;
		final_path = tmp + request.path;
	}
	else if (!config_data->all_locations[location_key].alias.empty())
	{
		std::string &tmp = config_data->all_locations[location_key].alias;
		final_path = tmp + request.path.substr(location_key.length());
	}
	else
	{
		final_path = request.path; // i should add it to the root of server;
	}
}

bool		client::is_status_ok(void) const
{
	return (code_status && code_status != 200 && code_status != 201);
}

void		client::remove_from_back(std::string &s, int n)
{
	while (n--)
		s.pop_back();
}

const std::string	client::serve_client(std::string data, int recvd)
{
	if (flags.is_request_body)
	{
		request.received_body_size += recvd;
		if (request.received_body_size > config_data->limit_body_size)
		{
			flags.request_finished = true;
			code_status = 413;
		}
	}
	if (!flags.request_finished)
	{

		request.parse(data);
		return answer_response;
	}
	else
	{
		if (request.output_file.is_open())
			request.output_file.close();
		if (request.method == "GET")
		{				
			response.get_method();
			if (flags.response_finished)
				answer_response += "\r\n";
		}
		else if (request.method == "POST")
		{
			response.post_method();
		}
		return answer_response;
	}
}


/*
<!DOCTYPE html><html><head><title>Error 404 - Page Not Found</title></head><body><h1>Error 404</h1></body></html>

*/