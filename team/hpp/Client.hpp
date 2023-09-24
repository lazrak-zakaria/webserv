#ifndef	CLIENT_HPP__
#define CLIENT_HPP__

#include "./headers.hpp"
#include "./MimeAndError.hpp"
#include "./ServerConfig.hpp"


class Client
{
	private:

		ServerConfig	*_configData;
		MimeAndError	*_mimeError;
		std::string		_finalPath;
		std::string		_locationKey;
		std::string		_finalAnswer;
		u_int16_t		_codeStatus;

		struct Request
		{
			std::string											method;
			std::string											path;
			std::string											query;
			std::string											requestHeader;
			std::map<std::string, std::vector<std::string> >	requestHeadersMap;

			std::string							requestBody;
			size_t								receivedSize;
			size_t								contentLength;			
			std::ofstream						outputFile;
			std::string							outputFileName;
	
			Client								*me;

			void	parseRequest(std::string &requestData, int received);
			void	parseHeader(size_t crlf);
			bool	isUriValid(const char &c) const;
			bool	isFieldNameValid(const char &c) const;
			bool	isFieldValueValid(const char &c) const;

			void	parseMultipart();
			void	parseChunkedData();


			void	setMe(Client *);

		} _request;


		struct Flags
		{
			bool	isRequestBody;
			bool	isChunked;
			bool	isMultipart;
			bool	inMultipartHeader;
		} _flags;

		bool	isLocationMatched(const std::string &locationDirective, const std::string &p);
		void	detectFinalLocation(void);


	public:

		void	test();


		void	setConfigData(ServerConfig	*c);
		void	setMimeError(MimeAndError	*m);
		bool	isRequestFinished() const;


		void			readRequest(std::string &RequestData, size_t recevivedSize);
		std::string		&serveResponse(void);
};

#endif