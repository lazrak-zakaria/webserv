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

		size_t			_timeLastAction;


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
			std::string							firstBoundary;
			std::string							endBoundary;
			std::string							contentType;
	
			size_t								readAmountSoFar;
			size_t								expectedBytesToRead;
			size_t								TotalDataProcessed;

			size_t								requestTimeStart;
			Client								*me;

			void	parseRequest();
			void	parseHeader(size_t crlf);
			bool	isUriValid(const char &c) const;
			bool	isFieldNameValid(const char &c) const;
			bool	isFieldValueValid(const char &c) const;

			void	parseMultipart();
			bool	parseMultipartHeader(size_t start, size_t crlfPos);
			void	parseChunkedData();

			void	requestClear();
			void	setMe(Client *);


			Request();
		} _request;

		struct Response
		{
			std::string				responseHeader;
			size_t					chunkedSize;

			std::ifstream			inputFile;

			std::string				location301;

			
			Client					*me;

			void					postMethodeResponse();
			void					postMethodeResponseDirectory();
			void					postMethodeResponseFile();
	
			void					sendFileToFinalAnswer();
			void					responseError();					
			void					generateResponseErrorHeader(void);
			void					responseClear();
			void					setResponseFinished(u_int8_t);
			std::string				getContentTypeOfFile(std::string &f);
		} _response;


		struct Cgi
		{
			pid_t		processPid;
			size_t		timeStart;

			std::string	inputFileCGi;
			std::string	outputFileCGi;
			
			std::string cgiKeyProgram;
			void		checkCgiTimeout();


			Client 		*me;
			void		executeCgi();
		} _cgi;

		struct Flags
		{
			/*request*/
			bool	isRequestFinished;

			bool	isRequestBody;
			bool	isChunked;
			bool	isMultipart;
			bool	inMultipartBody;
			bool	multicanw;
			/*chunked*/
			bool	expectSizeRead;
			bool	crlfRequired;

			/*response*/
			bool	canReadInputFile;
			bool	isResponseFinished;
			bool	isHeaderResponseSent;
			/*cgi*/
			bool	isCgiRunning;
		} _flags;

		bool	isLocationMatched(const std::string &locationDirective, const std::string &p);
		void	detectFinalLocation(void);
		void	addSlashToFinalPath();
		bool	isMatchedWithCgi(std::string &file);
		bool	isPathExist(std::string path);
		size_t	getTimeNow();
	public:

		Client();
		void	clearClient();

		void	test();
		void	generateRandomName(std::string &name) const;

		void	setConfigData(ServerConfig	*c);
		void	setMimeError(MimeAndError	*m);
		bool	isRequestFinished() const;


		void			readRequest(const char * RequestData, int recevivedSize);
		std::string		&serveResponse(void);
};	

#endif