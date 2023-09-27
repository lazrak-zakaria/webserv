#include "../hpp/Client.hpp"

void	Client::Response::setResponseFinished(u_int8_t code)
{
	me->_codeStatus = code;
	me->_flags.isResponseFinished = true;
}

void	Client::Response::sendFileToFinalAnswer()
{
	const int BUF_SIZE = 5000; 
	char	buf[BUF_SIZE + 2];
	inputFile.read(buf, BUF_SIZE);

	int	howMuchRead = inputFile.gcount();
	std::stringstream ss;
	ss << std::hex << howMuchRead;
	me->_finalAnswer = "";
	me->_finalAnswer.append(ss.str().append("\r\n"));

	buf[howMuchRead] = '\r';
	buf[howMuchRead + 1] = '\n';

	me->_finalAnswer.append(buf, howMuchRead + 2);

	if (howMuchRead != BUF_SIZE && howMuchRead)
	{
		me->_finalAnswer.append("0\r\n");
		inputFile.close();
		me->_flags.isResponseFinished = true;
	}
}

void	Client::Response::postMethodeResponse()
{
	if (me->_flags.canReadInputFile)
		sendFileToFinalAnswer();
	else if (me->_flags.isCgiRunning)
	{

	}
	else
	{
		if (me->_configData->allLocations[me->_locationKey].canUpload) // add more flags
		{
			/*201 created*/
			std::stringstream ss;
			ss << "HTTP/1.1 " << me->_codeStatus << " " << me->_mimeError->statusCode[me->_codeStatus] << "\r\n";
			ss << "Location: " << me->_request.path + "/"+ me->_request.outputFileName << "\r\n\r\n";

			me->_finalAnswer = ss.str();
			me->_flags.isRequestFinished = true;
			return ;
		}

		struct stat sb;
		if (stat(me->_finalPath.c_str(), &sb) == 0)
		{
			if (S_ISDIR(sb.st_mode))
			{
				/*end with slash */
				/*later*/

					if (me->_configData->allLocations[me->_locationKey].cgi.empty() == 0)
					{
						me->addSlashToFinalPath();
						if (me->_configData->allLocations[me->_locationKey].index.empty() == 0)
						{
							std::vector<std::string> &indexes =  me->_configData->allLocations[me->_locationKey].index;
							size_t i = 0;
							for (; i < indexes.size(); ++i)
							{
								if (me->isMatchedWithCgi(indexes[i]) && me->isPathExist(std::string(me->_finalPath).append(indexes[i])))
									break;
							}
							if (i == indexes.size())
							{
								goto END_RESPONSE;;
							}
							else
							{
								// me->_finalPath.append(indexes[i]);
								/* run cgi*/
								std::cout << "i will run cgi\n"; exit(0);
							}

						}
						else
						{
							goto END_RESPONSE;
						}
					}
					else
					{
						goto END_RESPONSE;
					}
			}
			else if (S_ISREG(sb.st_mode))
			{
					if (me->_configData->allLocations[me->_locationKey].cgi.empty() == false)
					{
						if (me->isMatchedWithCgi(me->_finalPath))
						{
							std::cout << me->_finalPath<< "--\n";
						}
						else
						{
							goto END_RESPONSE;
						}
					}
					else
					{
						goto END_RESPONSE;
					}
			}
		}
		else
		{
			me->_codeStatus = 404;
			me->_flags.isResponseFinished = true;
		}
	}

	return ;


	END_RESPONSE:
		me->_codeStatus = 403;
		me->_flags.isResponseFinished = true;
		std::cout << "ERROR\n";
}

void	Client::Response::responseError()
{
	if (me->_flags.canReadInputFile)
		Client::Response::sendFileToFinalAnswer();
	else
		generateResponseErrorHeader();
}

std::string	Client::Response::getContentTypeOfFile(std::string &f)
{
	size_t	pos = f.find_last_of('.');
	if (pos != std::string::npos)
	{
		std::string extension = f.substr(pos);
		if (me->_mimeError->mime.count(extension))
			return me->_mimeError->mime[extension];
	}
	return std::string("application/octet-stream");
}

void	Client::Response::generateResponseErrorHeader(void)
{
	std::stringstream ss;
	ss << "HTTP/1.1 " << me->_codeStatus << " " << me->_mimeError->statusCode[me->_codeStatus] << "\r\n";
	ss << "Transfer-Encoding: " << "chunked" << "\r\n";

	if (me->_request.requestHeadersMap.count("connection"))
	{
		std::string &tmp = * (--(me->_request.requestHeadersMap["connection"].end()));
		for (int i = 0; tmp[i] ; ++i)
			tmp[i] = tolower(tmp[i]);
		if (tmp.find("close") != std::string::npos)
			ss << "Connection: " << "close" << "\r\n";
		else
			ss << "Connection: " << "keep-alive" << "\r\n";
 	}
	else
			ss << "Connection: " << "keep-alive" << "\r\n";
	
	if (me->_configData->errorPages.count(me->_codeStatus))
	{
		inputFile.open(me->_configData->errorPages[me->_codeStatus].c_str());
		if (!inputFile.is_open())
		{
		}
		ss << "Content-Type: " << getContentTypeOfFile(me->_configData->errorPages[me->_codeStatus]) << "\r\n";
	}
	else
	{
		inputFile.open(me->_mimeError->errors[me->_codeStatus].c_str());
		if (!inputFile.is_open())
		{
		}
		ss << "Content-Type: " << "text/html" << "\r\n";
	}

	ss << "\r\n";
	me->_flags.canReadInputFile = true;
	// me->_flags.removeMe = true;

	me->_finalAnswer = ss.str();
}
