#ifndef REQUEST_HPP__
#define REQUEST_HPP__

#include "header.hpp"

class request
{
	private:
	
	public:
		std::map<std::string, std::vector<std::string> > header_fields;

		std::string		header;
		std::string		body;
		FILE*			temp_file;

		size_t			content_length;
		struct uri
		{
    		std::string methode;
			std::string http_version;
			std::string path;
			std::string query_string;
			//std::string fragment_identifier; //i see no use;
			std::string code;
		} uri;

		bool	is_header;
		bool	is_chunked;
		void	parse_uri(std::string first_line_request);
		void	parse_header();
		void	parse_body();
		void	parse_form_data(std::string &body_data);
		void	parse(std::string request_data);

		void	parse_chunked_data(std::string &body_data);
		void	parse_chunked(bool size_data, size_t pos);

		request(/* args */);
		~request();
};


#endif
