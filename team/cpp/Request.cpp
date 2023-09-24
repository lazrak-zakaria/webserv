#include "../hpp/Client.hpp"

// https://stackoverflow.com/questions/20457437/does-stdstringclear-reclaim-the-memory-associated-with-a-string

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
	static char token[] = "- \t._~:/?#[]@!$&'()*+,;=%";
	return (isalnum(c) || memchr(token, c, sizeof(token)));
}

void	Client::Request::parseHeader(size_t crlf)
{
	enum 
	{
		eUri,
		eMethode,
		ePath,
		eSlash,
		eQuery,
		eFragement,
		eHttp,
		eCr,
		eLf,
		eFieldName,
		eFieldNameChar,
		eFieldValue,
		eFieldValueChar
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
						me->_codeStatus = 400;
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
						me->_codeStatus = 400;
					break;
				case eQuery:
					if (currentChar == ' ')
						cursor = eHttp;
					else if (currentChar == '#')
						cursor = eFragement;
					else if (isUriValid(currentChar))
						query.push_back(currentChar);
					else
						me->_codeStatus = 400;
					break;
				case eFragement:
					if (currentChar == ' ')
						cursor = eHttp;
					else if (isUriValid(currentChar))
						continue;
					else
						me->_codeStatus = 400;
					break;
		
				case eHttp:
				{
					std::string	tmp;
					for (u_int8_t j = 0; j < 8 && i < requestHeader.size(); ++i, ++j)
						tmp.push_back(requestHeader[i]);
					if (tmp != "HTTP/1.1" || requestHeader[i] != '\r')
						me->_codeStatus = 400;
					cursor = eLf;
				}
					break;

				case eLf:
					if (currentChar != '\n')
						me->_codeStatus = 400;
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
					me->_codeStatus = 400;
				break;
			case eLf:
				if(currentChar != '\n')
					me->_codeStatus = 400;
				// goto done;
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
					{

						std::cout <<"++-\n";
						me->_codeStatus = 400;
					}
					break;
				case eLf:
					if (currentChar != '\n')
						me->_codeStatus = 400;
					else
					{
						cursor = eFieldNameChar;
						currentState = eFieldName;
					}
			}
			break;
		}
	}

	if (!requestHeadersMap.count("host") || requestHeadersMap["host"].size() != 1)
		me->_codeStatus = 400;

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
			{
				me->_codeStatus = 400;
			}
			if (tmp.size() > 19)
			{
				me->_codeStatus = 413;
			}
			contentLength = atoi(tmp.c_str());			
		}
	}

	if (requestHeadersMap.count("transfer-encoding"))
	{
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
			return ;
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

		if (contentType != "multipart/form-data;")
		{
			if (i != contentTypeTmp.size())
				me->_codeStatus = 400;
			return ;
		}

		if (contentTypeTmp.compare(i, 9, "boundary=") != 0)
		{
			me->_codeStatus = 400;
			return ;
		}

		i += 10;
		if (contentTypeTmp[i] == '\"'){
			for (; contentTypeTmp[i] != '\"' && i < contentTypeTmp.size(); ++i)
				boundary.push_back(contentTypeTmp[i]);
		}
		else {
			for (; contentTypeTmp[i] != ' ' && i < contentTypeTmp.size(); ++i)
				boundary.push_back(contentTypeTmp[i]);
		}

		for (; contentTypeTmp[i] == ' '; ++i) ;
		if (i != contentTypeTmp.size() || boundary.empty())
		{
			me->_codeStatus = 501;
			return ;
		}
		me->_flags.isMultipart = true;
	}

	// done:
	if (me->_codeStatus == 400 || me->_codeStatus == 413)
		std::cout << "Error\n";
	else
	{
		std::cout << "Method: " << method << "\n";
		std::cout << "Path: " << path << "\n";
		std::cout << "Query: " << query << "\n";

		for (auto it = requestHeadersMap.begin(); it != requestHeadersMap.end(); ++it)
		{
			std::cout << it->first << "|\t" ;
			for (auto j = it->second.begin(); j != it->second.end(); ++j)
				std::cout << *j << "|\t";
			std::cout << "\n";
		}
	}
	exit(0);
}

void	Client::test()
{
		_request.me = this;
		_codeStatus = 0;
	_request.requestHeader = "GET /ggg?rr HTTP/1.1\r\n\
kEy:  value\r\n\
Content-length: 544444\r\n\
host: c\r\n\
host: ca\r\n\
key1:value1\r\n\r\n";


	_request.parseHeader(94);
}

void	Client::Request::parseRequest(std::string &requestData, int received)
{
	if (me->_flags.isRequestBody)
	{
		if (received)
			requestBody.append(requestData, received);
		
		if (me->_configData->allLocations[me->_locationKey].cgi.empty() == 0)
		{
			if (me->_flags.isChunked)
				parseChunkedData();
			else
				outputFile.write(requestData.c_str(), received);
		}
		else if (me->_configData->allLocations[me->_locationKey].canUpload)
		{		
			if (me->_flags.isChunked)
				parseChunkedData();
			else if (me->_flags.isMultipart && me->_configData->allLocations[me->_locationKey].cgi.empty())
				parseMultipart();
			else
				outputFile.write(requestData.c_str(), received);
		}
		else
		{
			me->_codeStatus = 403;
			me->_flags.isRequestFinished = true;
		}
	}
	else
	{
		size_t searchEndOfHeader = requestHeader.size();
		searchEndOfHeader = (searchEndOfHeader > 7) ? searchEndOfHeader - 7 : 0; /*where to start searching*/
		requestHeader.append(requestData, received);

		size_t	posCrlf = requestHeader.find("\r\n\r\n", searchEndOfHeader);
		if (posCrlf == std::string::npos)
			return ;
	
		me->_flags.isRequestBody = true;
		requestBody = requestHeader.substr(posCrlf + 4);
		parseHeader(posCrlf);
		me->detectFinalLocation();

		if (!requestHeader.empty())
			std::string().swap(requestHeader);

		if (me->_configData->allLocations[me->_locationKey].cgi.empty() == 0)
		{
			// if ()
		}
		if (!requestBody.empty())
		{
			std::string ss;
			receivedSize = requestBody.size();
			parseRequest(ss, 0);
		}
	}
}

void	Client::Request::parseMultipart()
{
	size_t	bodyLength = requestBody.size();

	//parse multipartHeader
	if (!me->_flags.inMultipartBody && bodyLength >= boundary.length())
	{
		std::string firstBoundary  = std::string("--").append(boundary);
		std::string	finalBoundary = firstBoundary.append("--\r\n");
		size_t		finalBoundaryLine = requestBody.find(firstBoundary);

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
			size_t		firstBoundaryLine = requestBody.find(firstBoundary.append("\r\n"));
			if (firstBoundaryLine != std::string::npos)
			{
				if (parseMultipartHeader(firstBoundaryLine + 2, multipartCompleteHeader + 4) == false)
				{
					if (me->_codeStatus != 500)
						me->_codeStatus = 400;
					me->_flags.isRequestFinished = true;
					return ;
				}
			}
			requestBody.erase(0, multipartCompleteHeader + 4);
			me->_flags.inMultipartBody = true;
		}
	}


	// write body to file
	bool	bodyEnd = false;
	if (me->_flags.inMultipartBody)
	{
		std::string crlfAndBoundary =  std::string("\r\n").append("--").append(boundary) ;
		size_t	isBoundaryFound = requestBody.find(crlfAndBoundary);

		if (isBoundaryFound != std::string::npos)
		{
			if (outputFile.is_open())
			{
				outputFile.write(requestBody.substr(0, isBoundaryFound).c_str(), isBoundaryFound);
				outputFile.close();
			}
			requestBody = requestBody.substr(isBoundaryFound);
			me->_flags.inMultipartBody = false;
			bodyEnd = true;
		}
		else if (bodyLength > boundary.size() + 10 && outputFile.is_open())
		{
			size_t sizeToWrite = bodyLength - boundary.size() + 10;
			outputFile.write(requestBody.substr(0, sizeToWrite).c_str(), sizeToWrite);
			requestBody = requestBody.substr(sizeToWrite);
		}
	}


	if (requestBody.find("\r\n") && bodyEnd)
		parseMultipart();
}


// https://datatracker.ietf.org/doc/html/rfc7578#section-4.2
bool	Client::Request::parseMultipartHeader(size_t start, size_t multipartHeaderCrlf)
{
	enum
	{
		eContentDisposition,
		eName,
		eCheck,
		eFilename,
		eCr,
		eLf,
		eContentTypeOptional,
		eFinish
	};

	std::string	contentType;
	bool		isFileNameEmpty = true;
	u_int8_t	cursor = eContentDisposition;
	bool		optionalContentType = 0;
	for (int i = start; i < multipartHeaderCrlf; ++i)
	{
		switch (cursor)
		{
		case eContentDisposition:
		{
			std::string tmp;
			for ( ; i < 42 && i < multipartHeaderCrlf; ++i)
				tmp.push_back(tolower(requestBody[i]));
			if (tmp != "content-disposition: form-data; name=")
				return false;
			cursor = eName;
		}
		break;

		case eName:
		{
			if (requestBody[i++] != '\"')
				return false;
			for (; i < multipartHeaderCrlf; ++i)
				if (requestBody[i] == '\"')
					break;
			if (requestBody[i] != '\"')
				return 0;
			cursor = eCheck;
		}
		break;

		case eCheck:
		{
			if (requestBody[i] == '\r')
			{
				if (requestBody[i + 1] != '\n')
					return false;
				cursor = eFinish;
			}
			else if (requestBody[i] != ';')
				cursor = eFilename;
			else
				return 0;
		}
		break;

		case eFilename:
		{
			if (requestBody[i++] != ' ')
				return false;
			if (requestBody.compare(i, 8, "filename=") == 0)
			{
				i += 8;
				if (requestBody[i++] != '\"')
					return false;
				for (; i < multipartHeaderCrlf; ++i)
				{
					if (requestBody[i] == '\"')
						break;
					isFileNameEmpty = false;
				}
				if (requestBody[i] != '\"')
					return false;
				cursor = eCr;
				optionalContentType = true;
			}
		}
		break;

		case eCr:
		{
			if (requestBody[i] != '\r')
				return false;
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
				return true;
			if (tmp != "content-type")
				return false;
			else
			{
				while (i < multipartHeaderCrlf)
				{
					if (requestBody[i] == '\r')
						break;
					contentType.push_back(requestBody[i++]);
				}
			}
			--i;
			cursor = eFinish;
		}
		break;

		case eFinish:
		{
			if (requestBody.compare(i, 2, "\r\n") == 0)
				return true;
			return false;
		}
			break;
		}
	}

	if (!isFileNameEmpty)
	{
		std::string	tmpFileName;
		me->generateRandomName(tmpFileName);
		
		std::string tmpPath = me->_finalPath + tmpFileName;
		/*should i check if it exist ?*/

		outputFile.open(tmpPath, std::ios::binary);
		if (outputFile.is_open() == 0)
		{
			std::cerr << "File did not open\n";
			me->_codeStatus = 500;
			return false;
		}
	}
}
