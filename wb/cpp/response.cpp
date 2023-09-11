#include "../response.hpp"


void	response::create_header()
{
	std::stringstream ss;
	ss << response_line << "\r\n";
	// ss << "Date: " << date << "\r\n";
	// ss << "Last-Modified: " << last_modified << "\r\n";
	ss << "Content-Type: " << content_type << "\r\n";
	ss << "Content-Length: " << content_length << "\r\n";
	header = ss.str();
}



void	response::serve_response(server& server_conf, request& http_request)
{
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
	// std::string &path = http_request.uri.path;
	if (!body_sending)
	{
		struct stat	path_stat;
		if (!stat(final_path.c_str(), &path_stat))
		{
			if (S_ISREG(path_stat.st_mode))
			{
				//cgi later;
				ifs.open(final_path);
				if (!ifs.is_open())
				{
					//permision denied
				}
				//	after sending header turn on body_sending;

				content_length = path_stat.st_size;
				content_type = getmime(final_path);
				//	build header;
				body_sending = true;
				answer = header;
			}
			else if (S_ISDIR(path_stat.st_mode))
			{
				if (server_conf.all_locations[key_location].index.size())
				{
					if (final_path[final_path.length() - 1] != '/')
						final_path += '/';
					size_t size_ = server_conf.all_locations[key_location].index.size();
					std::vector<std::string> &index_files =  server_conf.all_locations[key_location].index;
					for (size_t i = 0; i < size_; ++i)
					{
						struct stat path_stat_2;
						std::string	tmp_path = final_path + index_files[i];
						if (!stat(tmp_path.c_str(), &path_stat_2))
						{
							if (S_ISREG(path_stat.st_mode))
							{
								//cgi later;
								ifs.open(final_path);
								if (!ifs.is_open())
								{
									//permision denied
								}
								//	after sending header turn on body_sending;

								content_length = path_stat.st_size;
								content_type = getmime(final_path);
								//	build header;
								body_sending = true;
								answer = header;
							}
						}
					}
					if (!body_sending) // meannig no index is valid
					{
						//error
					}

				}
				else if (server_conf.all_locations[key_location].directory_listing)
				{
					//answer = // generate auto index;
					finished_flag = true;

				}
			}
		}
	}
	else
	{
		char	buf[SIZE_READ];
		ifs.read(buf, SIZE_READ - 1);
		buf[ifs.gcount()] = '\0';
		received_from_file += ifs.gcount();
		finished_flag = received_from_file == content_length;
		if (finished_flag)
			ifs.close();
		answer = buf;
	}
}

/*
because im 
*/

std::string		response::getmime(std::string path)
{
	size_t	pos  = path.find_last_of('.');
	if (pos != std::string::npos)
	{
		std::string	tmp = path.substr(pos);
		if (mime.count(tmp))
			return mime[tmp];
	}
	return "application/octet-stream";
}




response::response()
{
	mime[".css"]	= "text/css";
	mime[".csv"]	= "text/csv";
	mime[".gif"]	= "image/gif";
	mime[".htm"]	= "text/html";
	mime[".html"]	= "text/html";
	mime[".ico"]	= "image/x-icon";
	mime[".jpeg"]	= "image/jpeg";
	mime[".jpg"]	= "image/jpeg";
	mime[".json"]	= "application/json";
	mime[".png"]	= "image/png";
	mime[".pdf"]	= "application/pdf";
	mime[".svg"]	= "image/svg+xml";
	mime[".txt"]	= "text/plain";
}

void	response::post_method(server& server_conf, request& http_request)
{
	location &valid_location = server_conf.all_locations[http_request.key_location];
	//  i should check if everything okay till this point;

	// if okay then
	if (!start_reading_file_cgi)
	{

		if (!valid_location.cgi.empty())
		{
			//handle cgi;
			// if ok open the file cgi output
			start_reading_file_cgi = true;
		}
		else
		{
			// build answer on the code_from request; that is : what the upload did
		}
	}
	else if (start_reading_file_cgi && !valid_location.cgi.empty())
	{
		char	buf[SIZE_READ];
		from_cgi_to_client.read(buf, SIZE_READ - 1);
		buf[ifs.gcount()] = '\0';
		received_from_file += ifs.gcount();
		finished_flag = !ifs.gcount();
		if (finished_flag)
			from_cgi_to_client.close();
		answer = buf;
	}
}
