#ifndef REQUEST_HPP__
#define REQUEST_HPP__

#include "./header.hpp"
#include "./server.hpp"

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
			// std::string code;
		} uri;

		std::ofstream	ofs;
		size_t			boundary_size;
		std::string		boundary;
		bool			flag;

		bool	is_header;
		bool	is_chunked;
		bool	is_multipart;
		void	parse_uri(std::string &first_line_request);
		void	parse_header();
		void	parse_body();
		void	parse_form_data(std::string &body_data);
		void	parse(std::string request_data, server& server_config);

		void	parse_chunked_data(std::string &body_data);
		void	parse_chunked(bool size_data, size_t pos);

		bool	finished_flag; // not added to the code yet

		request& operator=(const request& f);
		request(/* args */);
		request(const request& );

		~request();


		int	code_status;

		std::string		key_location;	// the valid location
		std::string		final_path; // path i deal with it
		void			detect_final_location(server& server_conf);
		bool			start_with(const std::string &location_directive, const  std::string &path);
		std::string		file_name;

		std::string		cgi_output_file_name;
		std::ifstream	from_cgi_to_client;
};

// https://www.jmarshall.com/easy/http/#postmethod

// https://www.filibeto.org/aduritz/misc/web_upload_cgi.html  cgi
#endif
