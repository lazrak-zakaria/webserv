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
