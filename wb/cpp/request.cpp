#include "../request.hpp"


void	request::parse(std::string request_data)
{
	if (!is_header)
	{
		if (is_chunked)
		{
			parse_chunked_data(request_data);
		}
		else
		{
			parse_form_data(request_data);
		}
	}
	else
	{
		header += request_data;
		size_t	pos = header.find("\r\n");
		if (pos == std::string::npos)
			return ;
		is_header = !is_header;
		body = header.substr(pos + 2);
		parse_header();
	}
}

void	request::parse_header()
{
	std::stringstream	ss(header);
	std::string			temp_data;
	
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
			if (pos = body.find("\r\n") != std::string::npos)
				parse_chunked(size_data ,pos);
}


void	parse_form_data(std::string &body_data)
{

}


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