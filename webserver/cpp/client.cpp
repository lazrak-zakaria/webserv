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
	return (path.substr(0, location_length) == location_directive); 
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
		final_path = tmp + request.path.substr(tmp.length());
	}
	else
	{
		final_path = request.path;
	}
}

void		client::remove_from_back(std::string &s, int n)
{
	while (n--)
		s.pop_back();
}

const std::string	&client::serve_client(const std::string data, int recvd)
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
		if (request.method == "GET")
		{
			
			response.get_method();
		}
		else if (request.method == "POST")
		{
			response.post_method();
		}
		return answer_response;
	}
}





/**********				request				**********/

client::request::request()
{
	content_length = std::string::npos;
	received_body_size = 0;
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
			if (!me->flags.tmp_file_open)
			{
				std::string tmp_name = "fileXXXXXX";
				std::string	tmp;
				if (valid_location.upload_store)
				{
					me->final_path += (me->final_path[me->final_path.length() - 1] != '/') ? "/" : ""; 
					tmp = me->final_path + tmp_name;
					me->code_status = 201;
				}
				else
				{
					tmp = "./Temporary/";
					tmp += tmp_name;
				}
				char *a = new char[tmp.length() + 1];
				memcpy(a, tmp.c_str(), tmp.length() + 1);
				int fd = mkstemp64(a);
				if (fd == -1)
				{
					me->code_status = 500;
					me->flags.request_finished = true;
					delete []a;
					return ;
				}
				close(fd);
				file_name = a;
				output_file.open(a);
				delete []a;
				if (!output_file.is_open())
				{
					me->code_status = 500;
					me->flags.request_finished = true;
					return ;
				}
				me->flags.tmp_file_open = true;
				if (!valid_location.cgi.empty())
					output_file << request_header << "\r\n";
			}
			if (me->flags.is_chunked)
			{
				if (!me->code_status)
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
				if (!request_data.empty())
				{
					output_file << request_data;
				}
				if (received_body_size >= content_length)
				{
					me->flags.request_finished = true;
				}
			}
		}
		else
		{
			me->code_status = 403; // Forbidden
			me->flags.request_finished = true;
		}
	}
	else
	{
		request_header += request_data;
		size_t	pos = request_header.find("\\r\\n\\r\\n");
		if (pos == std::string::npos)
			return ;
		me->flags.is_request_body = true;
		request_body = request_header.substr(pos + 8);
		request_header = request_header.substr(0, pos + 4);
		parse_header();		
		if (request_headers.count("content-length"))
		{
			if (content_length > me->config_data->limit_body_size)
			{
				me->flags.request_finished = true;
				me->code_status = 413; // Payload Too Large
				return ;
			}
		}
		print_header();
		me->detect_final_location();

		if (method != "POST")
		{
			me->flags.request_finished = true;
		}
		if (!request_body.empty())
		{
			parse("");
		}
	}
}

void	client::request::print_header()
{
	std::cout << "method: " << method << "\n";
	std::cout << "path: " << path << "\n";
	std::cout << "querystaring: " << query_string << "\n";
	std::cout << "http-version: " << http_version << "\n";

	for (auto it = request_headers.begin(); it != request_headers.end(); ++it)
	{
		std::cout << it->first << " " << it->second << "\n"; 
	}
}

void	client::request::parse_uri(std::string &uri)
{
	std::stringstream	ss(uri);
	std::string			temp_data;
	getline(ss, temp_data, ' ');
	if (temp_data != "GET" && temp_data != "POST" && temp_data != "DELETE")
	{
		me->flags.is_header_ok = false;
		me->flags.request_finished = true;
		me->code_status = 501; //Not Implemented
		return ;
	}
	method = temp_data;
	getline(ss, temp_data, ' ');
	if (temp_data.empty())
	{
		me->flags.is_header_ok = false;
		me->flags.request_finished = true;
		me->code_status = 4; //not suppoerted
		return ;
	}
	path = temp_data;
	getline(ss, temp_data);
	if (temp_data != "HTTP/1.1")
	{
		me->flags.is_header_ok = false;
		me->flags.request_finished = true;
		me->code_status = 400; //bad request
		return ;
	}
	http_version = temp_data;
	size_t i = 0;
	temp_data = path;
	path = "";
	for (i = 0 ; temp_data[i] && temp_data[i] != '?' && temp_data[i] != '#'; ++i)
		path += temp_data[i];
	if (temp_data[i] == '?')
	{
		for ( ; temp_data[i] && temp_data[i] != '#'; ++i)
			path += temp_data[i];
	}
	else if (temp_data[i] == '#')
	{
		for ( ; temp_data[i]; ++i)
			path += temp_data[i];
	}
	// i need to know more about path
}

void	client::request::parse_header(void)
{
	// std::string			request_header = request_header;
	// std::cout << request_header << "++\n"; exit(0);
	// me->remove_from_back(request_header, 4);
	std::stringstream	ss(request_header);
	std::string			tmp_string;

	getline(ss, tmp_string);
	me->remove_from_back(tmp_string, 4);
	parse_uri(tmp_string);
	bool	&is_header_ok = me->flags.is_header_ok;
	while (is_header_ok && getline(ss, tmp_string))
	{
		me->remove_from_back(tmp_string, 4);
		std::stringstream	stream(tmp_string);
		std::string			tmp_string_2;
		std::string			key_field;
		getline(stream, key_field, ' ');
		for (size_t i = 0; i < key_field[i]; ++i)
			key_field[i] = tolower(key_field[i]);
		getline(stream, tmp_string_2);
		request_headers[key_field] = tmp_string_2; 
		// print_header();
	}
	if (request_headers.count("transfer-encoding"))
	{
		if (request_headers["transfer-encoding"] == "chunked")
		{
			me->flags.is_chunked = true;
		}
		else
		{
			me->code_status = 504; //Not Implemented
			me->flags.request_finished = true;
		}
	}
	else if (request_headers.count("content-length:"))
	{

		content_length = atoi(request_headers["content-length:"].c_str());
		std::cout << content_length << "++\n";
	}
	else if (method != "POST" && (request_headers.count("transfer-encoding")
			|| request_headers.count("content-length")))
	{
		me->flags.request_finished = true;
		me->code_status = 403; //Forbidden
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
	received_from_input_file = 0;
}

void	client::response::generate_header(void)
{

}

void	client::response::generate_directory_listing(void)
{
	std::string	buf;
	DIR *dir;
	struct dirent *ent;
	if ((dir = opendir (me->final_path.c_str())) != NULL)
	{
	/* print all the files and directories within directory */
		while ((ent = readdir (dir)) != NULL)
		{
			buf += ent->d_name;
			buf += "\n";
		}
		std::stringstream ss;
		ss << buf.length() << "\r\n";
		closedir (dir);
		me->answer_response += response_header;
		me->answer_response += buf;
	} 
	else
	{
		me->code_status = 403; //forbiden
		me->answer_response = response_error();
	}
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
						me->code_status = 403;
						me->answer_response = response_error();
						me->flags.response_body_sending = true;
					}
					else
					{
						generate_header();
						content_length_input_file = path_stat.st_size;
						me->answer_response = response_header;
						me->flags.response_body_sending = true;
					}
				}
			}
			else if (S_ISDIR(path_stat.st_mode))
			{

				size_t	index_size = me->config_data->all_locations[me->location_key].index.size();
				if (me->final_path[me->final_path.length() - 1] != '/')
				{
					me->final_path += '/';		
				}			
				if (index_size)
				{
					std::vector<std::string> &index_files = me->config_data->all_locations[me->location_key].index;
					for(size_t i = 0; i < index_size; ++i)
					{
						struct stat	path_stat_2;
						std::string	tmp_final_path = me->final_path + index_files[i];
						if (!stat(tmp_final_path.c_str(), &path_stat_2))
						{
							if (S_ISREG(path_stat_2.st_mode))
							{
								if (!me->config_data->all_locations[me->location_key].cgi.empty())
								{
									
								}
								else
								{
									input_file.open(tmp_final_path);
									if (!input_file.is_open())
									{
										
									}
									else
									{
										generate_header();
										me->flags.response_body_sending = true;
										me->answer_response = response_header;
										content_length_input_file = path_stat_2.st_size;
									}
								}
							}
						}
						
					}
					if (!me->flags.response_body_sending)
					{
						me->code_status = 403; // later
						me->answer_response =  "error";// response_error();
						me->flags.response_finished = true;
					}
				}
				else if (me->config_data->all_locations[me->location_key].directory_listing)
				{
				}
				else
				{
					me->code_status = 403; // later
					me->answer_response = "error 403 forbiden\n";
					me->flags.response_finished = true;
				}
			}
		}
		else
		{
			me->code_status = 403; // later
			me->answer_response = response_error();
		}
	}
	else
	{
		char	buf[SIZE_READ];
		input_file.read(buf, SIZE_READ - 1);
		buf[input_file.gcount()] = '\0';
		received_from_input_file += input_file.gcount();
		me->flags.response_finished = received_from_input_file == content_length_input_file;
		if (me->flags.response_finished)
		{
			input_file.close();
		}
		me->answer_response = buf;
	}
}

void	client::response::post_method(void)
{
	location	&valid_location = me->config_data->all_locations[me->location_key];
	if (!me->flags.start_reading_cgi_output)
	{
		if (!valid_location.cgi.empty())
		{
			// me->cgi.execute_cgi();
			// if (me->code_status == 500 || me->code_status == 404)
			// 	me->answer_response = response_error();
			// me->flags.start_reading_cgi_output = true;
		}
		else
		{
			// me->answer_response = response_error(); // not an error
			me->flags.response_finished = true;
		}

	}
	else
	{
		char	buf[SIZE_READ];
		input_file.read(buf, SIZE_READ - 1);
		buf[input_file.gcount()] = '\0';
		received_from_input_file += input_file.gcount();
		me->flags.response_finished = received_from_input_file == content_length_input_file;
		if (me->flags.response_finished)
		{
			input_file.close();
		}
		me->answer_response = buf;
	}
}

std::string	client::response::response_error(void)
{

	return "";
}

/**********				cgi				**********/

namespace timeout
{
    int child_done = 0;
    int timeout = 0;
    void child_handler(int sig)
    {
        child_done = 1;
    }

    void alarm_handler(int sig)
    {
        timeout = 1;
    }

    void    siglisten()
    {
        signal(SIGALRM, alarm_handler);
        signal(SIGCHLD, child_handler);
    }
// https://stackoverflow.com/questions/40223722/how-to-stop-a-child-process-if-not-completed-after-a-timeout
    int   settimeout(int t, pid_t pid)
    {
        alarm(t);  // install an alarm to be fired after TIME_LIMIT
        pause();
        if (timeout)
        {
            int result = waitpid(pid, NULL, WNOHANG);
            if (result == 0)
                kill(pid, 9); // child still running, so kill it
            // else he terminate normally
			return 0;
		}
        else if (child_done)
        {
			wait(NULL); //printf("child finished normally\n");
			return 1;
		}
		return 1;
	}
};

// http://isp.vsi.ru/library/Perl/CGI/ch6.htm
void	client::cgi::execute_cgi(void)
{
	env_variables["SERVER_NAME: "] = me->config_data->host;
	env_variables["SERVER_PROTOCOL: "] = "HTTP/1.1";
	// env_variables["SERVER_PORT"] = me->config_data->port;
	env_variables["REQUEST_METHOD: "] = me->request.method;
	// env_variables["PATH_INFO"];
	if (me->request.request_headers.count("content-length"))
		env_variables["CONTENT_LENGTH: "] = me->request.request_headers["content-length"];
	if (me->request.request_headers.count("content-type"))
	{
		env_variables["CONTENT_TYPE: "] = me->request.request_headers["content-type"];
	}
	if (!me->request.query_string.empty())
		env_variables["QUERY_STRING: "] =  me->request.query_string;



	std::string toexec__[2];
	/*
		make sure final path not end with /

	*/
	size_t	pos = me->final_path.find_last_of('/') + 1;
	std::string	scriptname = me->final_path.substr(pos);
	toexec__[1] = scriptname;
	pos = scriptname.find_last_of('.');
	if (pos == std::string::npos)
	{
		me->code_status = 404;
		return ;
	}
	toexec__[0] = scriptname.substr(pos+1).c_str();

	char a[] = "./Temporary/fileXXXXXX";
	int fd = mkstemp64(a);
	if (fd == -1)
	{
		me->code_status = 500;
		return ;
	}
	output_file_name = a;
	pid_t child_pid = fork();
	if (!child_pid)
	{
		char **env= new char*[env_variables.size() + 1];
		size_t	i = 0;
		std::map<std::string , std::string>::iterator it;
		for (it = env_variables.begin(); it != env_variables.end(); ++it)
		{
			std::string tmp = it->first + it->second;
			env[i++] = strdup(tmp.c_str());
		}
		env[i] = NULL;
		freopen64(me->request.file_name.c_str(), "r", stdin);//check
		freopen64(a, "w", stdout);//check
		std::string  correct_directory = me->final_path.substr(me->final_path.find_last_of('/'));
		chdir(correct_directory.c_str());//check
		char *texec[3] = {strdup(toexec__[0].c_str()), strdup(toexec__[1].c_str()), NULL};
		execve(texec[0], texec, env);
		exit(0);
	}
	timeout::settimeout(2, child_pid);

}

