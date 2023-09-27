#include "../hpp/Client.hpp"

// https://stackoverflow.com/questions/20457437/does-stdstringclear-reclaim-the-memory-associated-with-a-string


Client::Request::Request() :receivedSize(0), 
						contentLength(0), readAmountSoFar(0), 
						expectedBytesToRead(0), TotalDataProcessed(0)
{
}

void	Client::Request::requestClear()
{
		method.clear();
		path.clear();
		query.clear();
		requestHeader;
		requestHeadersMap;

		std::string().swap(requestBody);
		receivedSize = 0;
		outputFile.close();
		outputFileName.clear();

		contentLength = 0;
		boundary.clear();
		contentType.clear();
	
		readAmountSoFar = expectedBytesToRead = TotalDataProcessed = 0;
}

void	Client::Request::setMe(Client *m)
{
	me = m;
}

bool	Client::Request::isUriValid(const char &c) const
{
	static char token[] = "-._~:/?#[]@!$&'()*+,;=%";
	return (isalnum(c) || memchr(token, c, sizeof(token)));
}

bool	Client::Request::isFieldNameValid(const char &c) const
{
	static char token[] = "-_";
	return (isalnum(c) || memchr(token, c, sizeof(token)));
}

bool	Client::Request::isFieldValueValid(const char &c) const
{
	static char token[] = "- \t._~:/?#[]@!$&'()*+,;=%\"";
	return (isalnum(c) || memchr(token, c, sizeof(token)));
}



void	Client::Request::parseRequest()
{
	PARSE_REQUEST:

	if (me->_flags.isRequestBody)
	{
		if (me->_configData->allLocations[me->_locationKey].cgi.empty() == 0)
		{
			/* open temporary file to store */
		}
		else if (me->_configData->allLocations[me->_locationKey].canUpload)
		{	
			if (!me->_flags.isMultipart && outputFile.is_open() == 0)
			{
				struct stat sb;
				if (stat(me->_finalPath.c_str(), &sb) == 0)
				{
					if (S_ISDIR(sb.st_mode))
						me->addSlashToFinalPath();
					else
					{
						me->_codeStatus = 403;
						me->_flags.isRequestFinished = true;
						return ;
					}
				}
				else
				{
					me->_codeStatus = 404;
					me->_flags.isRequestFinished = true;
					return ;
				}

				std::string name;
				me->generateRandomName(name);
				me->addSlashToFinalPath();
				std::string extension = me->_mimeError->mimeReverse.count(contentType) ? 
												me->_mimeError->mimeReverse[contentType] : "";
				me->_finalPath.append(name.append(extension));
				outputFileName = name;
				
				outputFile.open(me->_finalPath.c_str(), std::ios::binary);
				if (outputFile.is_open() == 0)
				{
					me->_codeStatus = 500;
					me->_flags.isRequestFinished = true;
					return ;
				}
				me->_codeStatus = 201;
			}

			if (me->_flags.isChunked)
				parseChunkedData();
			else if (me->_flags.isMultipart && me->_configData->allLocations[me->_locationKey].cgi.empty())
			{
				parseMultipart();
			}
			else
				outputFile.write(requestBody.c_str(), requestBody.size());
		}

	}
	else
	{
		size_t searchEndOfHeader = 0;//requestHeader.size();
		// searchEndOfHeader = (searchEndOfHeader > 7) ? searchEndOfHeader - 7 : 0; /*where to start searching*/


		size_t	posCrlf = requestHeader.find("\r\n\r\n");
		if (posCrlf == std::string::npos)
			return ;
	


		parseHeader(posCrlf + 4);
		if (me->_codeStatus)
		{
			me->_flags.isRequestFinished = true;
			return ;
		}

		me->detectFinalLocation();
		if (me->_codeStatus)
		{
			me->_flags.isRequestFinished = true;
			return ;
		}

		struct stat sb;
		if (me->isPathExist(me->_finalPath) == false)
		{
			me->_flags.isRequestFinished = true;
			me->_codeStatus = 404;
			std::cout << "I WILL STOP YOU HERE\n";
			return;
		}

		me->_flags.isRequestBody = true;
		requestBody = requestHeader.substr(posCrlf + 4);

		
		std::string().swap(requestHeader);

		if (!requestBody.empty())
		{
			receivedSize = requestBody.size();
			goto PARSE_REQUEST;
		}
	}
}


void	Client::Request::parseHeader(size_t crlf)
{
	enum 
	{
		eUri, eMethode, ePath, eSlash,
		eQuery, eFragement, eHttp,
		eCr, eLf, eFieldName, eFieldNameChar,
		eFieldValue, eFieldValueChar
	};
	std::string	key, value;
	u_int8_t	cursor = eMethode, currentState = eUri;
	bool		charAfterCrLf = false;


	for (size_t i = 0; i < crlf && !me->_codeStatus; ++i)
	{
		const char	&currentChar = requestHeader[i];


		switch (currentState)
		{
		case eUri:
			{
				switch (cursor)
				{
				case eMethode:
					while (requestHeader[i] >= 'A' && requestHeader[i] <= 'Z')
					{
						method.push_back(requestHeader[i++]);
					}
					me->_codeStatus = (requestHeader[i] != ' ') ? 400 : me->_codeStatus;
					cursor = eSlash;
					break;

				case eSlash:
					if (currentChar != '/')
						goto BAD_REQUEST;
					path.push_back(currentChar);
					cursor = ePath;
					break;
				case ePath:
					if (currentChar == '?')
						cursor = eQuery;
					else if (currentChar == '#')
						cursor = eFragement;
					else if (isUriValid(currentChar))
						path.push_back(currentChar);
					else if (currentChar == ' ')
						cursor = eHttp;
					else
						goto BAD_REQUEST;
					break;
				case eQuery:
					if (currentChar == ' ')
						cursor = eHttp;
					else if (currentChar == '#')
						cursor = eFragement;
					else if (isUriValid(currentChar))
						query.push_back(currentChar);
					else
						goto BAD_REQUEST;
					break;
				case eFragement:
					if (currentChar == ' ')
						cursor = eHttp;
					else if (isUriValid(currentChar))
						continue;
					else
						goto BAD_REQUEST;
					break;
		
				case eHttp:
				{
					std::string	tmp;
					for (u_int8_t j = 0; j < 8 && i < requestHeader.size(); ++i, ++j)
						tmp.push_back(requestHeader[i]);
					if (tmp != "HTTP/1.1" || requestHeader[i] != '\r')
						goto BAD_REQUEST;
					cursor = eLf;
				}
					break;

				case eLf:
					if (currentChar != '\n')
						goto BAD_REQUEST;
					currentState = eFieldName;
					cursor = eFieldNameChar;
				}
			}
			break;
		case eFieldName:
			switch (cursor)
			{
			case eFieldNameChar:
				if (currentChar == ':' && !key.empty())
				{
					currentState = eFieldValue ;
					cursor = eFieldValueChar;
				}
				else if (isFieldNameValid(currentChar))
				{
					key.push_back(tolower(currentChar));
					charAfterCrLf = true;
				}
				else if (currentChar == '\r' && charAfterCrLf)
					cursor = eLf;
				else
					goto BAD_REQUEST;
				break;
			case eLf:
				if(currentChar != '\n')
					goto BAD_REQUEST;
			}
			break;
		case eFieldValue:
			switch (cursor)
			{
				case eFieldValueChar:
					if (isFieldValueValid(currentChar))
						value.push_back(currentChar);
					else if (currentChar == '\r')
					{
						requestHeadersMap[key].push_back(value);
						key.clear();
						value.clear();
						cursor = eLf;
						charAfterCrLf = true;
					}
					else
						goto BAD_REQUEST;
					break;
				case eLf:
					if (currentChar != '\n')
						goto BAD_REQUEST;
					else
					{
						cursor = eFieldNameChar;
						currentState = eFieldName;
					}
			}
			break;
		}
	}

	if (method.compare("POST") && method.compare("GET") && method.compare("DELETE"))
	{
		me->_codeStatus = 501;
		goto BAD_REQUEST;
	}

	if (method.compare("POST") && (requestHeadersMap.count("content-length") || requestHeadersMap.count("transfer-encoding")))
	{
		me->_codeStatus = 413;
		goto BAD_REQUEST;
	}

	if (!requestHeadersMap.count("host") || requestHeadersMap["host"].size() != 1)
		goto BAD_REQUEST;
	

	if (requestHeadersMap.count("content-length"))
	{
		if (requestHeadersMap["content-length"].size() == 1)
		{
			size_t k = 0;
			std::string	&contentLengthTmp = requestHeadersMap["content-length"][0];
			while (k < contentLengthTmp.size() && contentLengthTmp[k] == ' ')
				++k;
			while (k < contentLengthTmp.size() && contentLengthTmp[k] == '0')
				++k;
			std::string	tmp;
			while (k < contentLengthTmp.size() && isdigit(contentLengthTmp[k]))
				tmp.push_back(contentLengthTmp[k++]);
			while (k < contentLengthTmp.size() && contentLengthTmp[k] == ' ')
				++k;
			if (k != contentLengthTmp.size())
				goto BAD_REQUEST;
			if (tmp.size() > 19)
			{
				me->_codeStatus = 413;
				goto BAD_REQUEST;
			}
			contentLength = atoi(tmp.c_str());			
		}
	}

	if (requestHeadersMap.count("transfer-encoding"))
	{
				// std::cout << "here\n";
		std::string &transferEncoding = *(--requestHeadersMap["transfer-encoding"].end());
		std::string	tmp;
		
		size_t i = 0;
		for (; transferEncoding[i] == ' '; ++i) ;
		for (; transferEncoding[i] != ' ' && i < transferEncoding.size(); ++i)
			tmp.push_back(transferEncoding[i]);
		for (; transferEncoding[i] == ' '; ++i) ;

		if (i != transferEncoding.size() || tmp != "chunked")
		{
			me->_codeStatus = 501;
				goto BAD_REQUEST;
		}

		me->_flags.isChunked = true;
	}

	if (requestHeadersMap.count("content-type"))
	{
		std::string &contentTypeTmp = *(--requestHeadersMap["content-type"].end());
		std::string	tmp;
		
		size_t i = 0;
		for (; contentTypeTmp[i] == ' '; ++i) ;
		for (; contentTypeTmp[i] != ' ' && i < contentTypeTmp.size(); ++i)
			contentType.push_back(contentTypeTmp[i]);
		for (; contentTypeTmp[i] == ' '; ++i) ;

		if (contentType.compare(0, 20,"multipart/form-data;") == 0)
		{
			if (contentTypeTmp.compare(i, 9, "boundary=") != 0)
				goto BAD_REQUEST;
			i += 9;
			if (contentTypeTmp[i] == '\"'){
				for (++i; contentTypeTmp[i] != '\"' && i < contentTypeTmp.size(); ++i)
					boundary.push_back(contentTypeTmp[i]);
				if (i == contentTypeTmp.size())
					goto BAD_REQUEST;
				++i;
			}
			else {
				for (; contentTypeTmp[i] != ' ' && i < contentTypeTmp.size(); ++i)
					boundary.push_back(contentTypeTmp[i]);
			}
			for (; contentTypeTmp[i] == ' '; ++i) ;
			if (i != contentTypeTmp.size() || boundary.empty())
				goto BAD_REQUEST;
			me->_flags.isMultipart = true;

			firstBoundary = std::string("--").append(boundary).append("\r\n");
			endBoundary = std::string("--").append(boundary).append("--\r\n");
		}

		if (i != contentTypeTmp.size())
				goto BAD_REQUEST;

		// std::cout << contentType << "["<< boundary<< "]\n";
	}


	
	if (requestHeadersMap.count("transfer-encoding"))
	{
		if (requestHeadersMap.count("content-length"))
			goto BAD_REQUEST;
	}

	if (requestHeadersMap.count("content-type"))
	{
		if (requestHeadersMap.count("transfer-encoding") && requestHeadersMap.count("content-length"))
			goto BAD_REQUEST;
	}



	/***********************************************************************************/
/*	if (me->_codeStatus)
		std::cout << "Error\n";
	else
	{
		std::cout << "Method: " << method << "\n";
		std::cout << "Path: " << path << "\n";
		std::cout << "Query: " << query << "\n";
		std::cout << "bpundary: " << boundary << "\n";

		for (auto it = requestHeadersMap.begin(); it != requestHeadersMap.end(); ++it)
		{
			std::cout << it->first << "|\t|" ;
			for (auto j = it->second.begin(); j != it->second.end(); ++j)
				std::cout << *j << "|\t";
			std::cout << "\n";
		}
	}*/
	/***********************************************************************************/




	return ;

	BAD_REQUEST:
		me->_codeStatus = me->_codeStatus ? me->_codeStatus : 400;
		std::cout << "Error bad request " << me->_codeStatus << "\n";
}


void	Client::Request::parseMultipart()
{
	MULTIPART_START:
	size_t	bodyLength = requestBody.size();

	if (!me->_flags.inMultipartBody && bodyLength >= boundary.length())
	{
		size_t		finalBoundaryLine = requestBody.find(endBoundary);
		if (finalBoundaryLine != std::string::npos)
		{
			if (requestBody.find("filename=") == std::string::npos)
			{
				outputFile.close();
				me->_flags.isRequestFinished = true;
				std::string().swap(requestBody);
				return ;
			}
		}

		size_t	multipartCompleteHeader = requestBody.find("\r\n\r\n");
		if (multipartCompleteHeader != std::string::npos)
		{
			size_t		firstBoundaryLine = requestBody.find(firstBoundary);
			if (firstBoundaryLine != std::string::npos)
			{
				if (parseMultipartHeader(firstBoundary.size(), multipartCompleteHeader + 4) == false)
				{
					if (me->_codeStatus != 500)
						me->_codeStatus = 400;
					exit(4);
					me->_flags.isRequestFinished = true;
					return ;
				}
			}
			else
			{
				me->_codeStatus = 400;
				me->_flags.isRequestFinished = true;
				return ;
			}
			requestBody.erase(0, multipartCompleteHeader + 4);
			me->_flags.inMultipartBody = true;
		}
	}

	bool	bodyEnd = false;
	if (me->_flags.inMultipartBody)
	{
		std::string crlfAndBoundary = std::string("\r\n").append("--").append(boundary);
		size_t	isBoundaryFound = requestBody.find(crlfAndBoundary);

		if (isBoundaryFound != std::string::npos)
		{
			// std::cout << requestBody.substr(0, isBoundaryFound) << "}\n";
			if (outputFile.is_open())
			{
				outputFile.write(requestBody.substr(0, isBoundaryFound).c_str(), isBoundaryFound);
				outputFile.close();
			}
			requestBody = requestBody.erase(0, isBoundaryFound+2);
			// std::cout << requestBody << ":\n";
			me->_flags.inMultipartBody = false;
			bodyEnd = true;
		}
		else if (requestBody.size() > boundary.size() + 10 && outputFile.is_open())
		{
			size_t sizeToWrite = requestBody.size() - boundary.size() + 10;
			outputFile.write(requestBody.substr(0, sizeToWrite).c_str(), sizeToWrite);
			requestBody = requestBody.erase(0, sizeToWrite);
		}
	}


	if (requestBody.find("\r\n") && bodyEnd)
		goto MULTIPART_START;

}


bool	Client::Request::parseMultipartHeader(size_t start, size_t multipartHeaderCrlf)
{
	enum
	{
		eContentDisposition, eName, eCheck, eFilename,
		eCr, eLf, eContentTypeOptional, eFinish
	};

	u_int8_t	cursor = eContentDisposition;
	bool		isFileNameEmpty = true;
	bool		optionalContentType = 0;
	std::string	contentTypeTmp;

	for (int i = start; i < multipartHeaderCrlf; ++i)
	{
		switch (cursor)
		{
		case eContentDisposition:
		{
			std::string tmp;
			for (int j = 0; j < 37 && i < multipartHeaderCrlf; ++i, ++j)
				tmp.push_back(tolower(requestBody[i]));
			if (tmp != "content-disposition: form-data; name=")
			{

				return false;
			}
			--i;
			cursor = eName;
		}
		break;

		case eName:
		{
			if (requestBody[i++] != '\"')
			{
				return false;
			}
			for (; i < multipartHeaderCrlf; ++i)
				if (requestBody[i] == '\"')
					break;
			if (requestBody[i] != '\"')
			{
				return 0;
			}
			cursor = eCheck;
		}
		break;

		case eCheck:
		{
			if (requestBody[i] == '\r')
			{
				if (requestBody[i + 1] != '\n')
				{
					return false;
				}
				++i;
				cursor = eFinish;
			}
			else if (requestBody[i] == ';')
			{
				cursor = eFilename;
				if (requestBody[++i] != ' ')
				{
					return false;
				}
			}
			else
			{

				return 0;
			}
		}
		break;

		case eFilename:
		{
			if (requestBody.compare(i, 9, "filename=") == 0)
			{
				i += 9;
				
				if (requestBody[i++] != '\"')
				{
					return false;
				}
				for (; i < multipartHeaderCrlf; ++i)
				{
					if (requestBody[i] == '\"')
						break;
					isFileNameEmpty = false;
				}
				if (requestBody[i] != '\"')
				{
					return false;
				}
				cursor = eCr;
				optionalContentType = true;
			}
			else
			{
				return false;
			}
		}
		break;

		case eCr:
		{
			if (requestBody[i] != '\r')
			{
				return false;
			}
			cursor = eLf;
		}
		break;

		case eLf:
		{
			if (requestBody[i] != '\n')
				return false;
			if (optionalContentType)
				cursor = eContentTypeOptional;
		}
		break;

		case eContentTypeOptional:
		{
			std::string tmp;
			int j = 0;
			for (; j < 12 && i < multipartHeaderCrlf; ++i, ++j)
				tmp.push_back(tolower(requestBody[i]));
			if (tmp == "\r\n")
			{
				return true;
			}
			if (tmp != "content-type")
				return false;
			else
			{
				++i;
				while (i < multipartHeaderCrlf)
				{
					if (requestBody[i] == '\r')
						break;
					contentTypeTmp.push_back(requestBody[i++]);
				}
			}
			if (requestBody.compare(i, 2, "\r\n") != 0)
			{
				return false;
			}
			i++;
			cursor = eFinish;
		}
		break;

		case eFinish:
		{
			if (requestBody.compare(i, 2, "\r\n") != 0)
			{
				std::cout << requestBody << "XX\n";
				return false;
			}
			++i;
		}
		break;
		}
	}

	if (!isFileNameEmpty)
	{
		std::string	tmpFileName;
		me->generateRandomName(tmpFileName);
		outputFileName = tmpFileName;
		std::string extension = me->_mimeError->mimeReverse.count(contentTypeTmp) ? 
												me->_mimeError->mimeReverse[contentTypeTmp] : "";

		std::string tmpPath = me->_finalPath + tmpFileName + extension;
		// /*should i check if it exist ?*/

		std::cout << tmpPath << "\n";
		outputFile.open(tmpPath, std::ios::binary);
		if (outputFile.is_open() == 0)
		{
			std::cerr << "File did not open\n";
			me->_codeStatus = 500;
			return false;
		}
		me->_codeStatus = 201;
		me->_flags.multicanw = true;
		std::cout << "CREAT FILE\n";
	}
	else 
		me->_flags.multicanw = false;

	return true;
}


void	Client::Request::parseChunkedData()
{
	START:

	if (me->_flags.expectSizeRead)
	{
		if (size_t pos = requestBody.find("\r\n") && pos != std::string::npos)
		{
			std::string hexValue(requestBody, 0, pos);
			
			std::stringstream ss;
			ss  << hexValue ; 
			ss >> std::hex >> expectedBytesToRead ;

			TotalDataProcessed += expectedBytesToRead;
			if (TotalDataProcessed > me->_configData->limitBodySize)
			{
				std::cout << "413 Payload Too Large\n";
				me->_codeStatus = 413;
				return ;
			}

			if (expectedBytesToRead == 0)
			{
				me->_flags.isRequestFinished = true;
				outputFile.close();
				return ;
			}

			requestBody.erase(0, pos + 2);
			readAmountSoFar = 0;
			me->_flags.expectSizeRead = false;
		}
	}
	else
	{
		size_t reqSize = requestBody.size();
		std::string	contentToStore;


		// /**/ this line is the idea
		// for (size_t i = 0 ; readAmountSoFar < expectedBytesToRead && i < reqSize; ++readAmountSoFar , ++i)
		// 		toWriteToFile.push_back(req[i]);
		// /**/

		if (!me->_flags.crlfRequired)
		{
			if (reqSize >= (expectedBytesToRead - readAmountSoFar))
			{
				contentToStore.append(requestBody, 0, (expectedBytesToRead - readAmountSoFar));

				requestBody.erase(0, (expectedBytesToRead - readAmountSoFar));
				readAmountSoFar = expectedBytesToRead = 0;
				me->_flags.crlfRequired = true;
			}
			else
			{
				contentToStore.append(requestBody);
				requestBody.erase(0, reqSize);
				readAmountSoFar += reqSize;

			}

			outputFile.write(contentToStore.c_str(), contentToStore.size());
		}


		if (me->_flags.crlfRequired && requestBody.size() >= 2)
		{
 			if (requestBody.compare(0, 2, "\r\n") == 0)
			{
				me->_flags.crlfRequired = false;
				requestBody.erase(0, 2);
			}
			else
			{
				// std::cout << "Error body should end with crlf\n";
				me->_codeStatus = 400;
			}

			me->_flags.expectSizeRead = true;
		}
	}

	//buffer still have data to be parsed
	if ((me->_flags.expectSizeRead && requestBody.find("\r\n")) || (me->_flags.expectSizeRead == 0 && requestBody.empty() == 0))
		goto START;
}
