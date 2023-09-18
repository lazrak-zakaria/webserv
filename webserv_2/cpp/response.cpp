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

void	client::response::generate_header(std::string &ph)
{
	std::stringstream ss;
	ss << "HTTP/1.1 OK 200\r\n";
	
	size_t	pos = ph.find_last_of('.');
	std::string c;
	if (pos != std::string::npos)
	{
		std::string s = ph.substr(pos+1);
		c = me->mime_status_code->mime.count(s) ? me->mime_status_code->mime[s] :  "application/octet-stream";
	}
	else
		c = "application/octet-stream";
	ss << "Content-Type: " << c << "\r\n";
	ss << "Content-Length: " << content_length_input_file << "\r\n";
	ss << "Connection: " << "Keep-Alive\r\n\r\n";
	me->answer_response = ss.str();

}

void	client::response::generate_directory_listing(void)
{

}


void	client::response::get_method(void)
{
	if (me->flags.response_body_sending)
	{
		char buf[50];
		input_file.read(buf, 49);
		buf[input_file.gcount()] = 0;
		received_from_input_file += input_file.gcount();
		me->flags.response_finished = received_from_input_file >= content_length_input_file;
		if (me->flags.response_finished)
			input_file.close();
		size_t	i = 0;
		size_t 	j = input_file.gcount();
		me->answer_response = "";
		while (i < j)
			me->answer_response.push_back(buf[i++]);
	}
	else
	{

		struct	stat sb;
		if (!stat(me->final_path.c_str(), &sb))
		{
			if(S_ISREG(sb.st_mode))
			{
				input_file.open(me->final_path.c_str(), std::ios::binary);
				if (!input_file.is_open())
				{

					me->code_status = 403;
					me->answer_response = response_error();
					me->flags.response_finished = true;
				}
				else
				{
					input_file.seekg (0, input_file.end);
					content_length_input_file = input_file.tellg();
					input_file.seekg (0, input_file.beg);
					generate_header(me->final_path);
					me->flags.response_body_sending = true;
				}
			}
			else if (S_ISDIR(sb.st_mode))
			{
				if (me->config_data->all_locations[me->location_key].index.size())
				{
					size_t	i = 0;
					for ( ; i < me->config_data->all_locations[me->location_key].index.size(); ++i)
					{
						std::string	tmp_path = me->final_path + '/' + me->config_data->all_locations[me->location_key].index[i];
						std::cout << tmp_path << "\n";
						struct	stat sb_2;
						if (!stat(tmp_path.c_str(), &sb_2))
						{
							if(S_ISREG(sb_2.st_mode))
							{
								input_file.open(tmp_path.c_str(), std::ios::binary);
								if (!input_file.is_open())
								{

									me->code_status = 403;
									me->answer_response = response_error();
									me->flags.response_finished = true;
								}
								else
								{
									input_file.seekg (0, input_file.end);
									content_length_input_file = input_file.tellg();
									input_file.seekg (0, input_file.beg);
									generate_header(tmp_path);
									me->flags.response_body_sending = true;
									break;
								}
							}
						}
					}
					if (me->flags.response_body_sending != true)
					{
						me->code_status = 403;
						me->answer_response = response_error();
						me->flags.response_finished = true;
					}
				}
				else if (me->config_data->all_locations[me->location_key].directory_listing)
				{

				}
				else
				{

				}
				
				
				
				
				/*
				me->final_path += '/' + me->config_data->all_locations[me->location_key].index[1];

				/******************
				input_file.open(me->final_path.c_str(), std::ios::binary);
				if (!input_file.is_open())
				{

					me->code_status = 403;
					me->answer_response = response_error();
					me->flags.response_finished = true;
				}
				else
				{
					input_file.seekg (0, input_file.end);
					content_length_input_file = input_file.tellg();
					input_file.seekg (0, input_file.beg);
					generate_header(me->final_path);
					me->flags.response_body_sending = true;
				/******************
				}
				*/
			}
			else
			{
				me->code_status = 403;
				me->answer_response = response_error();
				me->flags.response_finished = true;
			}
		}
		else
		{
			me->code_status = 404;
			me->answer_response = response_error();
			me->flags.response_finished = true;
		}

		/*
		{
		//GET /index.html http1.1

		// input_file.open(me->final_path, std::ios::binary);
		// if (!input_file.is_open())
		// 	exit(0);
		// std::stringstream ss;
		// ss << "HTTP/1.1 OK 200\r\n";
		// if (me->final_path == "/nfs/homes/zlazrak/Desktop/wbs/webfiles/index.html")
		// 	ss << "Content-Type: " << "text/html\r\n";
		// else 
		// 	ss << "Content-Type: " << "image/png\r\n";

		// input_file.seekg (0, input_file.end);
    	// content_length_input_file = input_file.tellg();
    	// input_file.seekg (0, input_file.beg);

		// ss << "Content-Length: " << content_length_input_file << "\r\n";
		// ss << "Connection: " << "Keep-Alive\r\n\r\n";
		// me->answer_response = ss.str();
		// me->flags.response_body_sending = true;
		}
		*/
	}
}

void	client::response::post_method(void)
{
	me->code_status = 404;
	me->answer_response = response_error();
	me->flags.response_finished = true;
}

std::string	client::response::response_error(void)
{
	std::stringstream ss;
	ss << "HTTP/1.1 " << me->code_status << " " << me->mime_status_code->status_code[me->code_status] << "\r\n";
	ss << "Content-Type: " << "text/html" << "\r\n";
	ss << "Content-Length: " << me->mime_status_code->errors[me->code_status].size() << "\r\n";
	ss << "Connection: " << "Close" << "\r\n" ; //later check if should check request
	ss << "\r\n";
	ss <<  me->mime_status_code->errors[me->code_status] << "\r\n";
	me->flags.remove_me = true;
	return ss.str();
}
