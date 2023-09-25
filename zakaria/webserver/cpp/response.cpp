#include "../client.hpp"


/**********				response				**********/

client::response::response()
{
	content_length_input_file = std::string::npos;
	received_from_input_file = 0;
}

void	client::response::res_clear()
{
	response_headers.clear();
	response_header.clear();
	content_length_input_file = std::string::npos;
	received_from_input_file = 0;
	input_file.close();
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
						std::stringstream ss;
						ss << "HTTP/1.1 200 OK\r\n";
						ss << "Content-Type: " << "text/html\r\n";
						ss << "Content-Length: " <<  path_stat.st_size << "\r\n";
						ss << "Connection: " <<  "Keep-Alive" << "\r\n\r\n";
						// generate_header();
						content_length_input_file = path_stat.st_size;
						me->answer_response = ss.str();
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
									input_file.open(tmp_final_path, std::ios::binary);
									// std::cout << tmp_final_path << ";;\n"; exit(3);
									if (!input_file.is_open())
									{
										
									}
									else
									{
										// content_length_input_file = path_stat.st_size;

										input_file.seekg (0, input_file.end);
    									content_length_input_file = input_file.tellg();
    									input_file.seekg (0, input_file.beg);
										std::stringstream ss;
										ss << "HTTP/1.1 200 OK\r\n";
										ss << "Content-Type: " << "text/html\r\n";
										ss << "Content-Length: " <<  path_stat.st_size << "\r\n";
										ss << "Connection: " <<  "Keep-Alive" << "\r\n\r\n";
										// generate_header();
										me->answer_response = ss.str();
										me->flags.response_body_sending = true;
										// generate_header();
										// me->flags.response_body_sending = true;
										// me->answer_response = response_header;
										// content_length_input_file = path_stat_2.st_size;
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
		size_t i = 0;
		size_t sss = input_file.gcount();
		me->answer_response = "";
		while (i < sss)
			me->answer_response.push_back( buf[i++]);
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
			// me->answer_response = response_error(); // not always an error
			me->flags.response_finished = true;
		}

	}
	else if (!input_file.is_open())
	{
		// me->answer_response = response_error(); // not always an error
		me->flags.response_finished = true;
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
	std::stringstream ss;
	ss << "HTTP/1.1 " << me->code_status << " " << me->mime_status_code->status_code[me->code_status] << "\r\n";
	ss << "Content-Type: " << "text/html" << "\r\n";
	ss << "Content-Length: " << me->mime_status_code->errors[me->code_status].size() << "\r\n";
	ss << "Connection: " << "close" << "\r\n" ; //later check if should check request
	ss << "\r\n";
	ss <<  me->mime_status_code->errors[me->code_status] << "\r\n";
	return ss.str();
}
