#ifndef	CLIENT_HPP__
#define CLIENT_HPP__

#include "./headers.hpp"
#include "./MimeAndError.hpp"
#include "./ServerConfig.hpp"


class Client
{
	private:

		std::map<std::string, ServerConfig*> _allConfigsData;
		ServerConfig	*_defaultConfigData;
		ServerConfig	*_configData;
		MimeAndError	*_mimeError;
		std::string		_finalPath;
		std::string		_locationKey;
		std::string		_finalAnswer;
		u_int16_t		_codeStatus;
		size_t			_timeLastAction;

		std::vector<std::string>			filesToDelete;
		struct stat 	_st;
		DIR 			*_FdDirectory;
		struct dirent	*_ReadDirectory;

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

			std::string				convertToHex(size_t sz);

			void					sendCgiHeaders();


			void					sendFileToFinalAnswer();
			void					responseError();
			void					generate200Header();					
			void					generateResponseErrorHeader(void);
			void					responseClear();
			void					setResponseFinished(u_int8_t);
			std::string				getContentTypeOfFile(std::string &f);

			void					GenerateLastResponseHeader(int status, std::string filename, struct stat *st);
			void					ErrorResponse();
			std::string				FindFileToOpen();

			// get
			void GetMethodResponse();
			void GetDirectory();
			void GetFile();
			std::vector<std::string> readdirectory();
			std::string generatehtml(std::vector<std::string> dir);
			void SendChunkDir();

			// delete
			void DeleteMethodResponse();
			int deletedir(std::string path);
			std::vector<std::string> DelReadDir(std::string path);			int delflag;


		} _response;


		struct Cgi
		{

			std::map<std::string, std::string>	cgiHeadersMap;
			std::string							cgiHeader;
			std::string							cgibody;
			std::string							statusLine;
			
			pid_t								processPid;
			size_t								timeStart;

			std::string	inputFileCGi;
			std::string	outputFileCGi;
			
			std::string cgiKeyProgram;
			void		checkCgiTimeout();	
			void		parseCgiHeader();
			void		sendCgiBodyToFinaleAnswer();

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
			bool	CanReadInputDir;
			bool	isResponseFinished;
			bool	isHeaderResponseSent;
			
			/*cgi*/
			bool	isCgiRunning;
			bool	isCgiFinished;     //set this flag if and only if cgi terminate properly
			bool	isCgiHeaderSent;
			bool	isCgiHaveContentLength;
			bool	isCgi;
		} _flags;

		bool	isLocationMatched(const std::string &locationDirective, const std::string &p);
		void	detectFinalLocation(void);
		void	addSlashToFinalPath();
		bool	isMatchedWithCgi(std::string &file);
		bool	isPathExist(std::string path);
		size_t	getTimeNow();
	public:

		Client();
		~Client();
		void	clearClient();

		void	test();
		void	generateRandomName(std::string &name) const;

		void	setConfigData(ServerConfig	*c);
		void	setMimeError(MimeAndError	*m);
		bool	isRequestFinished() const;
		bool	isResponseFinished() const;


		void			readRequest(const char * RequestData, int recevivedSize);
		std::string		&serveResponse(void);
};	

#endif