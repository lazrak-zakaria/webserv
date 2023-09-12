#include "../client.hpp"


client::client()
{
	config_data = NULL;
	memset(&flags, 0, sizeof(flags));
	request.me = this;
	response.me = this;
	cgi.me = this;
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

}

void		client::detect_final_location(void)
{

}

const std::string	&client::serve_client(const std::string data)
{

}





/**********				request				**********/

client::request::request()
{
		content_length = std::string::npos;
		received_body_size = 0;
		code_status = 0;
}

void	client::request::parse(const std::string &request_data)
{
	if (me->flags.is_request_body)
	{
		location &valid_location = me->config_data->all_locations[me->location_key]; 
		if (!valid_location.cgi.empty() 
				|| valid_location.upload_store)
		{
			//open file if cgi add file to delete later
			if (me->flags.is_chunked)
			{
			
				request_body += request_data;
				parse_chunked_data();
			}
			else
			{
				if (!request_body.empty())
				{
					output_file << request_body;
					request_body = "";
				}
				output_file << request_data;
			}
		}
		else
		{
			code_status = -9; // later
			me->flags.request_finished = true;
		}
	}
	else
	{
		request_header += request_data;
		size_t	pos = request_header.find("\r\n\r\n");
		if (pos == std::string::npos)
			return ;
		me->flags.is_request_body = true;
		request_body = request_header.substr(pos + 4);
		parse_header();
		if (request_headers.count("content-length"))
		{
			if (content_length > me->config_data->limit_body_size)
			{
				me->flags.request_finished = true;
				code_status = -77; // too large
				return ;
			}
		}
		me->detect_final_location();
	}
}

void	client::request::parse_uri(std::string &uri)
{

}

void	client::request::parse_header(void)
{
	std::stringstream	ss(request_header);
	std::string			tmp_string;

	getline(ss, tmp_string);
	tmp_string[tmp_string.length() - 1] = '\0';
	parse_uri(tmp_string);
	bool	&is_header_ok = me->flags.is_header_ok;
	while (is_header_ok && getline(ss, tmp_string))
	{
		tmp_string[tmp_string.length() - 1] = '\0';
		std::stringstream	stream(tmp_string);
		std::string			tmp_string_2;
		std::string			key_field;
		getline(stream, key_field);
		for (size_t i = 0; i < key_field[i]; ++i)
			key_field[i] = tolower(key_field[i]);
		getline(stream, tmp_string_2, '\r');
		request_headers[key_field] = tmp_string_2; 
	}
	if (request_headers.count("transfer-encoding"))
	{
		if (request_headers["transfer-encoding"] == "chunked")
		{
			me->flags.is_chunked = true;
		}
		else
		{
			code_status = -8; //later
			me->flags.request_finished = true;
		}
	}
	else if (request_headers.count("content-length"))
	{
		content_length = atoi(request_headers["content-length"].c_str());
	}
	else if (method != "POST" && (request_headers.count("transfer-encoding")
			|| request_headers.count("content-length")))
	{
		me->flags.request_finished = true;
		code_status = -7;
	}
	else
	{
		me->flags.request_finished = true;
	}
}


void	client::request::parse_chunked_data(void)
{
	size_t	pos = request_body.find("\r\n");
	parse_chunked(0, pos);
}


void	client::request::parse_chunked(bool size_or_data, size_t pos)
{
	 if (pos != std::string::npos && !size_or_data) //0 == hex_size    1 == data
	{
    	if (request_body == "0\r\n")
        {
        	request_body = "";
			return ;
    	}
    	request_body.erase(0, pos + 2);
    	size_or_data = 1;
	}
	else if (size_or_data && pos != std::string::npos)
	{
    	std::string g = request_body.substr(0, pos);
		output_file << g;
    	request_body = request_body.substr(pos + 2);
    	size_or_data = 0;
	}
	if (size_or_data && pos == std::string::npos && request_body.length() > 2)
	{
		std::string g = request_body.substr(0, request_body.length() - 2);
    	output_file << g;
    	request_body = request_body.substr(request_body.length() - 2);
	}
	if (pos != std::string::npos)
		if ((pos = request_body.find("\r\n")) != std::string::npos)
			parse_chunked(size_or_data ,pos);
}


/**********				response				**********/

client::response::response()
{
	content_length_input_file = std::string::npos;
}

void	client::response::generate_header(void)
{

}

void	client::response::generate_directory_listing(void)
{

}

void	client::response::get_method(void)
{
	if (!me->flags.response_body_sending)
	{
		struct stat	path_stat;
		if (!stat(me->final_path.c_str(), &path_stat))
		{
			if (S_ISREG(path_stat.st_mode))
			{
				if (!me->config_data->all_locations[me->location_key].cgi.empty())
				{

				}
				else
				{
					input_file.open(me->final_path.c_str());
					if (!input_file.is_open())
					{

					}
					else
					{
						generate_header();
						me->answer_response = response_header;
						me->flags.response_body_sending = true;
					}
				}
			}
			else if (S_ISDIR(path_stat.st_mode))
			{
				
			}
		}
	}
}

void	client::response::post_method(void)
{

}

/**********				cgi				**********/

void	client::cgi::execute_cgi(void)
{

}

