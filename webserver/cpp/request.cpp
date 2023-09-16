#include "../client.hpp"


/**********				request				**********/

client::request::request()
{
	content_length = std::string::npos;
	received_body_size = 0;
}

void	client::request::req_clear()
{
	request_headers.clear();
	boundary.clear();
	request_header.clear();
	request_body.clear();
	content_length = std::string::npos;
	received_body_size = 0;

	output_file.close();
	file_name.clear();
	for (size_t i = 0; i < files_to_delete.size(); ++i)
		remove(files_to_delete[i].c_str());
	files_to_delete.clear();
	method.clear();
	http_version.clear();
	path.clear();
	query_string.clear();
	ty.clear();
}

void	client::request::parse(std::string &request_data)
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
					tmp = ".././Temporary/";
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
				// if (!valid_location.cgi.empty())
				// {
				// 	output_file << request_header << "\r\n";
				// }
			}
			if (me->flags.is_chunked)
			{
				// if (!me->code_status)
					request_body += request_data;
				parse_chunked_data();
			}
			else if (me->flags.is_multipart)
			{
				// std::cout << request_body << "\n++\n"; sleep(1);// exit(0); 
				// if (!me->code_status)
					request_body += request_data;
				parse_form_data();
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
					//later see if ishould remove \r\n from back body
					// i think i should not if cgi;
					if (received_body_size >= content_length)
					{
						if (valid_location.cgi.empty() )
						{
							size_t t = received_body_size - content_length;
							while (t--)
								request_data.pop_back();
						}
						me->flags.request_finished = true;
					} 
					output_file << request_data;
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
		size_t	pos = request_header.find("\r\n\r\n");
		if (pos == std::string::npos)
			return ;
		me->flags.is_request_body = true;
		request_body = request_header.substr(pos + 4);
		request_header = request_header.substr(0, pos + 2);
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
		
		me->detect_final_location();
		// print_header();
		if (method != "POST")
		{
			me->flags.request_finished = true;
		}
		if (!request_body.empty())
		{
			received_body_size = request_body.size();
			parse(ty);
		}
	}
}

void	client::request::print_header()
{
	std::cout << "method: |" << method << "|\n";
	std::cout << "path: |" << path << "|\n";
	std::cout << "querystaring: |" << query_string << "|\n";
	std::cout << "http-version: |" << http_version << "|\n";

	for (auto it = request_headers.begin(); it != request_headers.end(); ++it)
	{
		std::cout << "|" << it->first << "| |" << it->second << "|\n"; 
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
	if (request_headers.count("content-type:"))
	{
		std::string	&content_type = request_headers["content-type:"];
		if (content_type.find("multipart/form-data") != std::string::npos)
		{
			size_t	pos_boundary = content_type.find("boundary=");
			pos_boundary += 9;
			char final_char_boundary = content_type[pos_boundary] == '\"' ? '\"' : '\0';
			// std::cout << final_char_boundary << "|\n"; exit(0);
			if (final_char_boundary == '\"') pos_boundary++;
			boundary = content_type.substr(pos_boundary, content_type.find(final_char_boundary, pos_boundary) - pos_boundary);

			me->flags.tmp_file_open = true; // to open file from the multipart;
			me->flags.is_multipart = true;
			// std::cout << "------------------------------------->: |" << boundary << "|\n"; exit(0);
		}
	}
	if (request_headers.count("transfer-encoding:"))
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
		// std::cout << content_length << "++\n";
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

void	client::request::parse_form_data()
{
	size_t	body_length = request_body.length();

	//header multipart here
	if (!me->flags.multipart_header && body_length >= boundary.length())
	{
		size_t	multipart_header_pos = request_body.find("\\r\\n");
		if (multipart_header_pos != std::string::npos)
		{
			 //exit(5);
			if (request_body.find(boundary + "--") != std::string::npos && request_body.find("filename=") == std::string::npos)
			{
			//  std::cout << "{\n" << request_body << "\n}\n\n\n"; 
				std::cout << "++"<< "\n";
				// exit(6);
				output_file.close();
				me->flags.request_finished = true;
				request_body = "";
				return ;
			}
			multipart_header_pos = request_body.find("\\r\\n\\r\\n");
			if (multipart_header_pos != std::string::npos)
			{

				std::string	request_header_multipart = request_body.substr(0, multipart_header_pos);
				if (request_header_multipart.find(boundary) != std::string::npos
					&& request_header_multipart.find("filename=") != std::string::npos)
				{
					std::cout << "{\n" << request_header_multipart << "\n}\n\n\n"; 

					size_t	cont_ty_pos = request_header_multipart.find("Content-Type: ");
					std::string	mimetype =  request_header_multipart.substr(cont_ty_pos + 14, multipart_header_pos - (cont_ty_pos + 14));
					std::cout << "-------------------------------------------------" << mimetype <<"----\n";
					if (output_file.is_open())
						output_file.close();
					std::string tmp_name = "fileXXXXXX";
					me->final_path += (me->final_path[me->final_path.length() - 1] != '/') ? "/" : ""; 
					tmp_name = me->final_path + tmp_name;
					std::string	suffix = me->mime_status_code->mime_reverse.count(mimetype) ? me->mime_status_code->mime_reverse[mimetype] : "";
					tmp_name += suffix;
					me->code_status = 201;
					char *a = new char[tmp_name.length() + 1];
					memcpy(a, tmp_name.c_str(), tmp_name.length() + 1);
					int fd = mkstemps64(a, suffix.length());	
					if (fd == -1)
					{
						me->code_status = 500;
						me->flags.request_finished = true;
						delete []a;
						return ;
					}
					close(fd);
					//rename(a, file_name.c_str());
					tmp_name = a;
					output_file.open(tmp_name.c_str());
					delete []a;
					if (!output_file.is_open())
					{

						me->code_status = 500;
						me->flags.request_finished = true;
						return ;
					}
				}
			}
			if (multipart_header_pos != std::string::npos)
			{
				request_body.erase(0, multipart_header_pos + 8);
				me->flags.multipart_header = true;
			}
			// !me->flags.multipart_header
		}

	}
	if (me->flags.multipart_header)
	{
		size_t	position_crlf = request_body.find("\\r\\n");
		if (position_crlf != std::string::npos)
		{
			if (output_file.is_open())
			{
				// std::cout << request_body << "\n"; sleep(2);
				std::string	tmp = request_body.substr(0, position_crlf);
				output_file << tmp;
				output_file.close();
			}
			request_body = request_body.substr(position_crlf + 4);
			me->flags.multipart_header = false;
			if (request_body.find("\\r\\n"))
				parse_form_data();
		}
		else if (body_length > boundary.size() && output_file.is_open())
		{
			std::string	tmp = request_body.substr(0, body_length - boundary.size());
			output_file << tmp;
			request_body = request_body.substr(tmp.length());
		}
	}
}
