#ifndef CLIENT_HPP__
#define CLIENT_HPP__

#include "server_config.hpp"

class client
{
	private:
		server_config		*config_data;

		std::string			location_key;
		std::string			final_path;

		std::string			answer_response;

		struct request
		{
			std::map<std::string, std::string>	request_headers;
			std::string							request_header;
			std::string							request_body;
			size_t								content_length;
			size_t								received_body_size;
			int									code_status;

			std::ofstream						output_file;
			std::vector<std::string>			files_to_delete;

			std::string							method;
			std::string							http_version;
			std::string							path;
			std::string							query_string;


			void	parse(const std::string &request_data);
			void	parse_uri(void);
			void	parse_header(void);
			void	parse_body(void);

			void	parse_chunked_data(void);
			void	parse_chunked(bool size_data, size_t pos);
			
			client								*me;
		} request;


		struct response
		{
			std::map<std::string, std::string>	response_headers;
			std::string							response_header;
			size_t								content_length_input_file;

			std::ifstream						input_file;
			
			void			generate_header(void);
			void			generate_directory_listing(void);
			void			get_method(void);
			void			post_method(void);
		// 	void			delete_method(void);


			client								*me;
		} response;

		struct cgi
		{
			std::map<std::string, std::string> env_variables;
			void	execute_cgi(void);

			client								*me;
		} cgi;

		struct  flags
		{
			bool	in_request;
			bool	in_response;

			bool	is_header;
			bool	is_chunked;

			bool	start_reading_cgi_output;

			bool	response_body_sending;

			bool	response_finished;
			bool	request_finished;
		} flags;



		void		detect_final_location(void);
		bool		start_with(const std::string &location_directive, const std::string &path);

		client(const client &);
		client & operator = (const client &);


	public:

		void	serve_client(const std::string data);

		client();
		~client();
};

#endif
