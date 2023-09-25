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
		request_body.append(request_data);
		parse_form_data();
	}
	else
	{
		request_header += request_data;
		size_t	pos = request_header.find("\r\n\r\n");
		if (pos == std::string::npos)
			return ;
		me->flags.is_request_body = true;
		request_body = request_header.substr(pos + 4);
		request_header = request_header.substr(0, pos + 2); // only 1 crlf in the end;
		parse_header();
		me->detect_final_location();
		print_header(); /*------------------------------------DBG*/

		if (!request_body.empty())
		{
			std::string ss;
			received_body_size = request_body.size();
			client::request::parse(ss);
		}


	}







	// request_header += request_data;
	// if (request_header.find("\r\n\r\n") != std::string::npos)
	// {
	// 	std::cout << "{\n" << request_header << "\n}\n";
	// 	me->flags.request_finished = true;
	// }
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
	std::cout << "{" << me->final_path << "}\n";
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
}

void	client::request::parse_header(void)
{
	std::stringstream	ss(request_header);
	std::string			tmp_string;

	getline(ss, tmp_string);
	me->remove_from_back(tmp_string, 1);
	parse_uri(tmp_string);

	bool	&is_header_ok = me->flags.is_header_ok;
	while (is_header_ok && getline(ss, tmp_string))
	{
		me->remove_from_back(tmp_string, 1);

		std::stringstream	stream(tmp_string);
		std::string			tmp_string_2;
		std::string			key_field;

		getline(stream, key_field, ' ');
		for (size_t i = 0; i < key_field[i]; ++i)
			key_field[i] = tolower(key_field[i]);
		getline(stream, tmp_string_2);
		request_headers[key_field] = tmp_string_2; 
	}

	/*
		post headers check here;
	*/
	if (request_headers.count("content-type:"))
	{
		std::string	&content_type = request_headers["content-type:"];
		if (content_type.find("multipart/form-data") != std::string::npos)
		{
			size_t	pos_boundary = content_type.find("boundary=");
			pos_boundary += 9;
			char final_char_boundary = content_type[pos_boundary] == '\"' ? '\"' : '\0';
			if (final_char_boundary == '\"')
				pos_boundary++;
			boundary = content_type.substr(pos_boundary, content_type.find(final_char_boundary, pos_boundary) - pos_boundary);
			me->flags.tmp_file_open = true;
			me->flags.is_multipart = true;
		}
	}
	if (request_headers.count("content-length:"))
	{
		content_length = atoi(request_headers["content-length:"].c_str());
		// std::cout << content_length << "++\n";
	}

	if (method != "POST")
		me->flags.request_finished = true;

}


void	client::request::parse_chunked_data(void)
{

}

void	client::request::parse_chunked(bool size_or_data, size_t pos)
{
	size_t	body_length = request_body.length();


}

void	client::request::parse_form_data()
{
	// std::cout << "here\n"; exit(7);
	size_t	body_length = request_body.size();

	if (!me->flags.multipart_header && body_length >= boundary.length())
	{
		size_t	multipart_header_pos = request_body.find("\r\n");
		
		if (multipart_header_pos != std::string::npos)
		{
			if (request_body.find(boundary + "--") != std::string::npos && request_body.find("filename=") == std::string::npos)
			{
				output_file.close();
				me->flags.request_finished = true;
				request_body = "";
				return ;
			}
		}
		multipart_header_pos = request_body.find("\r\n\r\n");
		if (multipart_header_pos != std::string::npos)
		{
			std::string	request_header_multipart = request_body.substr(0, multipart_header_pos);
			if (request_header_multipart.find(boundary) != std::string::npos
					&& request_header_multipart.find("filename=") != std::string::npos)
			{
				size_t cont_ty_pos = request_header_multipart.find("Content-Type: ") + 14; // check later valid
				std::string	mime_type;
				while (request_header_multipart[cont_ty_pos] != ' ' && request_header_multipart[cont_ty_pos] != ';'
					&& request_header_multipart[cont_ty_pos])
						mime_type += request_header_multipart[cont_ty_pos++];

				if (output_file.is_open())
					output_file.close();
				
				std::string tmp_name = "fileXXXXXX";
				me->final_path += (me->final_path[me->final_path.length() - 1] != '/') ? "/" : "";

				std::string	suffix = me->mime_status_code->mime_reverse.count(mime_type) ? me->mime_status_code->mime_reverse[mime_type] : "";
				tmp_name += suffix;
				
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
				tmp_name = a;
				output_file.open(tmp_name.c_str(), std::ios::binary);
				delete []a;
				if (!output_file.is_open())
				{
					me->code_status = 500;
					me->flags.request_finished = true;
					return ;
				}
			}
			if (multipart_header_pos != std::string::npos)
			{
				request_body.erase(0, multipart_header_pos + 4);
				me->flags.multipart_header = true;
			}
		}
	}
	if (me->flags.multipart_header)
	{
		std::string sd = "\r\n" ;
		sd = sd + "--" + boundary;
		size_t	position_crlf = request_body.find(sd);
		if (position_crlf != std::string::npos)
		{
			if (output_file.is_open())
			{
				std::cout << "-------------------\n";
				// std::cout << request_body << "\n"; sleep(2);
				std::string	tmp = request_body.substr(0, position_crlf);
				// std::cout << "//\n" <<  tmp << "//\n"; exit(8);
				output_file.write(tmp.c_str(), tmp.size());
				output_file.close();
			}
			request_body = request_body.substr(position_crlf + 2);
			me->flags.multipart_header = false;
			if (request_body.find("\r\n"))
				parse_form_data();
		}
		else if (body_length > boundary.size() + 9 && output_file.is_open())
		{
			std::string	tmp = request_body.substr(0, body_length - boundary.size());
			output_file.write(tmp.c_str(), tmp.size());
			request_body = request_body.substr(tmp.size());
		}
	}
}
