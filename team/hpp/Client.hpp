#ifndef	CLIENT_HPP__
#define CLIENT_HPP__

#include "./headers.hpp"

class Client
{
	private:

		u_int16_t	_codeStatus;

		struct Request
		{
			std::string							method;
			std::string							path;
			std::string							query;

			std::string							requestHeader;
			std::map<std::string, std::vector<std::string> >	requestHeadersMap;

			size_t								contentLength;			

			Client								*me;

			void	parseHeader(void);
			bool	isUriValid(const char &c) const;
			bool	isFieldNameValid(const char &c) const;
			bool	isFieldValueValid(const char &c) const;

			void	setMe(Client *);
			// Request();
		} _request;

		struct Flags
		{
			
		} _flags;

	public:

		void	test();
};


#endif