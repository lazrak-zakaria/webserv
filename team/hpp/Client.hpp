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
			std::ofstream						outputFile;
			std::string							outputFileName;

			size_t								contentLength;
			std::string							boundary;
			std::string							contentType;
	
			Client								*me;

			void	parseRequest(std::string &requestData, int received);
			void	parseHeader(size_t crlf);
			bool	isUriValid(const char &c) const;
			bool	isFieldNameValid(const char &c) const;
			bool	isFieldValueValid(const char &c) const;

			void	parseMultipart();
			bool	parseMultipartHeader(size_t start, size_t crlfPos);
			void	parseChunkedData();


			void	setMe(Client *);

		} _request;

		struct Response
		{
			std::string				responseHeader;
			size_t					chunkedSize;

			std::ifstream			inputFile;
			Client					*me;
			void					postMethodeResponse();
			void					responseError(void);
			void					clearResponse();
		} _response;

		struct Flags
		{
			/*request*/
			bool	isRequestFinished;
	
			bool	isRequestBody;
			bool	isChunked;
			bool	isMultipart;
			bool	inMultipartBody;

			/*response*/
			bool	canReadInputFile;
			bool	isResponseFinished;
		} _flags;

		bool	isLocationMatched(const std::string &locationDirective, const std::string &p);
		void	detectFinalLocation(void);


	public:

		void	test();
		void	generateRandomName(std::string &name) const;

		void	setConfigData(ServerConfig	*c);
		void	setMimeError(MimeAndError	*m);
		bool	isRequestFinished() const;


		void			readRequest(std::string &RequestData, size_t recevivedSize);
		std::string		&serveResponse(void);
};	

#endif