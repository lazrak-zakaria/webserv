#include "../request.hpp"


void	request::parse(std::string request_data, server& server_config)
{
	if (!is_header)
	{
		location &valid_location =  server_config.all_locations[key_location];
		if (!valid_location.cgi.empty()
				|| valid_location.upload_store)
		{
			if (!valid_location.cgi.empty())
			{
				// i will open a file in temporary_dir
				if (is_chunked)
				{
					parse_chunked_data(request_data);
				}
				else
				{
					ofs << request_data;
				}
			}
			else
			{
				// open file in the final_path
				if (is_chunked)
				{
					parse_chunked_data(request_data);
				}
				else if (is_multipart) // i will see what to do about it later
				{
					
				}
				else //just store it
				{
					ofs << request_data;
				}
			}
		}
		else
		{
			code_status = 90000; // what should i do ?
			finished_flag = true;
		}
	}
	else
	{
		header += request_data;
		size_t	pos = header.find("\r\n\r\n");
		if (pos == std::string::npos)
			return ;
		is_header = !is_header;
		body = header.substr(pos + 4);
		parse_header();
		detect_final_location(server_config);
	}
}

void	request::parse_header()
{
	std::stringstream	ss(header);
	std::string			temp_data;
	std::cout << "----------------\n";
	std::cout << header;
	std::cout << "----------------\n";

	getline(ss, temp_data);
	parse_uri(temp_data);

	while (getline(ss, temp_data))
	{
		temp_data[temp_data.size() - 1] = '\0';
		std::stringstream			string_input(temp_data);
		std::string					temp_data_2;
		std::string					key;
		getline(string_input, key, ' ');
		for(int i = 0; key[i]; ++i)
			key[i] = tolower(key[i]);
		while (getline(string_input, temp_data_2, ' '))
			header_fields[key].push_back(temp_data_2);
	}
	if (header_fields.count("transfer-encoding"))
	{
		size_t index = header_fields["transfer-encoding"].size() - 1;
		if (header_fields["transfer-encoding"][index] == "chunked")
			is_chunked = true;
		else
			throw std::runtime_error("Not supported");
	}
	else if (header_fields.count("content-length"))
	{
		content_length = atoi(header_fields["content-length"][1].c_str());
	}
	//else change to fdsetwrite;
}

void	request::parse_chunked_data(std::string &body_data)
{
	body += body_data;
	size_t pos = body.find("\r\n");
	parse_chunked(0, pos);
}

void	request::parse_chunked(bool size_data, size_t pos)
{
	    if (pos != std::string::npos && !size_data)
        {
            if (body[0] == '0')
            {
                body = "";
				return ;
            }
            body.erase(0, pos + 4);
            size_data = 1;
        }
        else if (size_data && pos != std::string::npos)
        {
            std::string g = body.substr(0, pos);
			fprintf(temp_file, "%s", g.c_str());
            body = body.substr(pos + 4);
            size_data = 0;
        }
		if (size_data && pos == std::string::npos && body.length() > 2)
		{
			std::string g = body.substr(0, body.length() - 2);
            fprintf(temp_file, "%s", g.c_str());
            body = body.substr(body.length() - 2);
		}
		if (pos != std::string::npos)
			if ((pos = body.find("\r\n")) != std::string::npos)
				parse_chunked(size_data ,pos);
}

// just in case of upload
void	request::parse_form_data(std::string &body_data)
{
	body += body_data;
	size_t	position_crlf;
	size_t	body_length = body.length();
	if (!flag && body_length + 4 >= boundary_size)
	{
		if (body.find(boundary) != std::string::npos)
		{
			size_t	boundary_position_and_header = body.find("\r\n\r\n");
			if (body.find("filename=") != std::string::npos)
			{
				//later i will see if use just random name or the name of file;
				char a[] = "fileXXXXXX";
				int fd = mkstemp(a);
				ofs.close();
				ofs.open(a);
				//check later//;
				body.erase(0, boundary_position_and_header + 4);
				flag = true;
			}
		}
	}
	if (flag)
	{
		position_crlf = body.find("\r\n");
		if (position_crlf != std::string::npos)
		{
			std::string	tmp = body.substr(position_crlf);
			ofs << tmp;
			body = body.substr(position_crlf);
			flag = false;
		}
		else if (body_length > boundary_size)
		{
			std::string	tmp = body.substr(0, body_length - boundary_size);
			ofs << tmp;
		}
	}
}


void	request::parse_uri(std::string &first_line_request)
{
	std::stringstream	ss(first_line_request);
	std::string			temp_data;
	getline(ss, temp_data, ' ');
	if (temp_data != "GET" && temp_data != "POST" && temp_data != "DELETE")
		throw std::runtime_error("Methode not allowed");
	uri.method = temp_data;
	getline(ss, temp_data, ' ');
	if (temp_data.empty())
		throw std::runtime_error("Request Not valid");
	uri.path = temp_data;
	getline(ss, temp_data, '\r');
	std::cout << "********************************\n|"<< temp_data << "|\n";
	if (temp_data != "HTTP/1.1")
		throw std::runtime_error("Not valid protocol");
	size_t i = 0;
	temp_data = uri.path;
	uri.path = "";
	for (i = 0 ; temp_data[i] && temp_data[i] != '?' && temp_data[i] != '#'; ++i)
		uri.path += temp_data[i];
	if (temp_data[i] == '?')
	{
		for ( ; temp_data[i] && temp_data[i] != '#'; ++i)
			uri.path += temp_data[i];
	}
	else if (temp_data[i] == '#')
	{
		for ( ; temp_data[i]; ++i)
			uri.path += temp_data[i];
	}
	// i need to know more about path
}



// char *tmpname = strdup("/tmp/tmpfileXXXXXX");
// mkstemp(tmpname);
// ofstream f(tmpname);



/*


POST /upload HTTP/1.1
Host: example.com
Content-Type: multipart/form-data; boundary=boundary123
Connection: close

--boundary123\r\n
Content-Disposition: form-data; name="field1"\r\n
\r\n
value1\r\n
--boundary123\r\n
Content-Disposition: form-data; name="field2"\r\n
\r\n
value2\r\n
--boundary123\r\n
Content-Disposition: form-data; name="file1"; filename="file.txt"\r\n
Content-Type: application/octet-stream\r\n
\r\n
[Contents of the file.txt here]\r\n
--boundary123--\r\n


*/


bool	request::start_with(const std::string &location_directive,const  std::string &path)
{
	size_t	location_length = location_directive.length();
	return (path.substr(0, location_length) == location_directive); // later check if ending with '/' or '\0'
}

void	request::detect_final_location(server& server_conf)
{
	std::map<std::string, location>::iterator	it;
	std::vector<std::string> location_set;
	for (it = server_conf.all_locations.begin(); it == server_conf.all_locations.end(); ++it)
	{
		if (start_with(it->first, uri.path))
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
	this->key_location = location_set[index_of_most_descriptive_location];
	if (!server_conf.all_locations[key_location].root.empty()) // not empty
	{
		std::string &tmp = server_conf.all_locations[key_location].root;
		this->final_path = tmp + uri.path;
	}
	else
	{
		std::string &tmp = server_conf.all_locations[key_location].alias;
		this->final_path = tmp + uri.path.substr(tmp.length());
	}
}



request::request(/* args */)
{

}
request::request(const request& f)
{
	*this = f;
}
request& request::operator=(const request& f)
{
		header_fields = f.header_fields;

		header = f.header;
		body = f.body;
		temp_file = f.temp_file;

		content_length = f.content_length;
		uri = f.uri;

		// ofs = f.ofs;
		boundary_size = f.boundary_size;
		boundary = f.boundary;
		flag = f.flag;

		is_header = f.is_header;
		is_chunked	= f.is_chunked;
		is_multipart = f.is_multipart;
		finished_flag = f.finished_flag;
		code_status	= f.code_status;

		key_location = f.key_location;
		final_path = f.final_path;
		file_name = f.file_name;
		cgi_output_file_name = f.cgi_output_file_name;
		// from_cgi_to_client = f.from_cgi_to_client;
	return *this;
}
request::~request()
{

}