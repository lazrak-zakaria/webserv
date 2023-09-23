#include "../hpp/Client.hpp"

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

void	Client::Request::parseHeader(void)
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
	for (size_t i = 0; i < requestHeader.size() && !me->_codeStatus; ++i)
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

	if (!requestHeadersMap.count("host") || requestHeadersMap["host"].size() != 1)
		me->_codeStatus = 400;


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


	_request.parseHeader();
}