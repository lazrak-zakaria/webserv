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
    		std::string method;
			std::string http_version;
			std::string path;
			std::string query_string;
			//std::string fragment_identifier; //i see no use;
			std::string code;
		} uri;

		std::ofstream	ofs;
		size_t			boundary_size;
		std::string		boundary;
		bool			flag;

		bool	is_header;
		bool	is_chunked;
		void	parse_uri(std::string &first_line_request);
		void	parse_header();
		void	parse_body();
		void	parse_form_data(std::string &body_data);
		void	parse(std::string request_data);

		void	parse_chunked_data(std::string &body_data);
		void	parse_chunked(bool size_data, size_t pos);

		bool	finished_flag; // not added to the code yet

		request& operator=(const request& f);
		request(/* args */);
		request(const request& );

		~request();
};


#endif
