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
	
}

