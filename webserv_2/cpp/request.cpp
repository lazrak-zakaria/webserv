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
		print_header();
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

	me->flags.request_finished = true;
}


void	client::request::parse_chunked_data(void)
{

}

void	client::request::parse_chunked(bool size_or_data, size_t pos)
{

}

void	client::request::parse_form_data()
{

}
