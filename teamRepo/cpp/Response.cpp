#include "../hpp/Client.hpp"

void	Client::Response::setResponseFinished(u_int8_t code)
{
	me->_codeStatus = code;
	me->_flags.isResponseFinished = true;
}

void	Client::Response::sendFileToFinalAnswer()
{
	const int 	BUF_SIZE = 5000; 
	char		buf[BUF_SIZE + 2];
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
		me->_flags.isResponseFinished = true; /*make sure this flag is only here*/
	}
}


void	Client::Response::postMethodeResponseDirectory()
{

	/*end with slash */
	/* MR YOYAHYA SAID WE SHOULD SEND REDIRECTION */
	// if (me->_finalPath[me->_finalPath.size() - 1] != '/') /* later 
	// {
	// 	me->_codeStatus = 301;
	// 	location301 = me->_request.path;
	// 	location301.push_back('/');
	// 	return ;
	// }


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
				{
					me->_finalPath.append(indexes[i]);
					break;
				}
			}

			/*no index match with cgi*/
			if (i == indexes.size())
			{
				std::cout << "no index match or exist with cgi-s\n";
				goto END_RESPONSE;;
			}
			else
			{
				// me->_finalPath.append(indexes[i]);
				/* run cgi*/
				std::cout << "-----\ni will run cgi\n"; 
				std::cout << "script file: " << me->_finalPath << "|\n";
				std::cout << "program: " << me->_cgi.cgiKeyProgram << "|\n-----\n";
				me->_cgi.executeCgi();

				exit(0);
			}

		}
		else
		{
			std::cout << "you requested a directory and did not provide any index \n";
			goto END_RESPONSE;
		}
	}
	else
	{
		std::cout << "you are doing a post request and you did not provide an upload nor cgi\n";
		goto END_RESPONSE;
	}


	return;

	END_RESPONSE:
		me->_codeStatus = 403;
		std::cout << "ERROR\n";
}

void	Client::Response::postMethodeResponseFile()
{
	if (me->_configData->allLocations[me->_locationKey].cgi.empty() == false)
	{
		if (me->isMatchedWithCgi(me->_finalPath)) /* if file does not exist le the child process quite with error*/
		{
			/*run cgi */
			std::cout << "-----\nfile i will run cgi\n"; 
			std::cout << "script file: " << me->_finalPath << "|\n";
			std::cout << "program: " << me->_cgi.cgiKeyProgram << "|\n";
			me->_cgi.executeCgi();
		}
		else
		{
			/*no cgi can run this*/
			std::cout << "you requested a file and did not matched with any cgi\n";
			goto END_RESPONSE;
		}
	}
	else
	{
		/* WTF do you want*/
		std::cout << "if you want a page do GET later i send you a file\n";
		goto END_RESPONSE;
	}

	return ;

	END_RESPONSE:
		me->_codeStatus = 403;
		std::cout << "ERROR\n";

}

void	Client::Response::sendCgiHeaders()
{
	
}

void	Client::Response::postMethodeResponse()
{
	// if (me->_flags.canReadInputFile && !me->_flags.isCgiFinished)
	// 	sendFileToFinalAnswer();
	if (me->_flags.isCgiRunning || me->_flags.isCgiFinished)
	{
		if (me->_flags.isCgiRunning)
			me->_cgi.checkCgiTimeout();
		if (me->_flags.isCgiFinished)
		{
			if (!me->_flags.isCgiHeaderSent)
			{
				me->_cgi.parseCgiHeader();
				me->_flags.isCgiHeaderSent = true;
			}
			else
			{
				me->_cgi.sendCgiBodyToFinaleAnswer();
			}
		}
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
			me->_flags.isResponseFinished = true;
			return ;
		}

		struct stat sb;
		if (stat(me->_finalPath.c_str(), &sb) == 0)
		{
			if (S_ISDIR(sb.st_mode))
			{
				postMethodeResponseDirectory();
			}
			else if (S_ISREG(sb.st_mode))
				postMethodeResponseFile();
			else
				me->_codeStatus = 403;
		}
		else
		{
			std::cout << "what you requested is not found\n";
			me->_codeStatus = 404;
		}
	}
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
		std::string extension = f.substr(pos + 1);
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
		{
			ss << "Connection: " << "close" << "\r\n";
		}
		else
		{
			ss << "Connection: " << "keep-alive" << "\r\n";
		}
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







/*****************************************************************************************/
/*****************************************************************************************/
/*****************************************************************************************/
/*****************************************************************************************/
/*****************************************************************************************/
/*****************************************************************************************/
/*****************************************************************************************/
/*****************************************************************************************/

void	Client::Response::generate200Header()
{
	std::stringstream ss;
	ss << "HTTP/1.1 " << me->_codeStatus << " " << "OK" << "\r\n";
	ss << "Transfer-Encoding: " << "chunked" << "\r\n";

	if (me->_request.requestHeadersMap.count("connection"))
	{
		std::string &tmp = * (--(me->_request.requestHeadersMap["connection"].end()));
		for (int i = 0; tmp[i] ; ++i)
			tmp[i] = tolower(tmp[i]);
		if (tmp.find("close") != std::string::npos)
		{
			ss << "Connection: " << "close" << "\r\n";
		}
		else
		{
			ss << "Connection: " << "keep-alive" << "\r\n";
		}
	}
	else
			ss << "Connection: " << "keep-alive" << "\r\n";


	ss << "Content-Type: " << getContentTypeOfFile(me->_finalPath) << "\r\n";


	ss << "\r\n";
	me->_flags.canReadInputFile = true;

	me->_finalAnswer = ss.str();
}			


void	Client::Response::getMethodeResponseDirectory()
{
	me->addSlashToFinalPath();
	if (me->_configData->allLocations[me->_locationKey].cgi.empty() == 0)
	{
		if (me->_configData->allLocations[me->_locationKey].index.empty() == 0)
		{
			std::vector<std::string> &indexes =  me->_configData->allLocations[me->_locationKey].index;
			size_t i = 0;
			for (; i < indexes.size(); ++i)
			{
				if (me->isMatchedWithCgi(indexes[i]) && me->isPathExist(std::string(me->_finalPath).append(indexes[i])))
				{
					me->_finalPath.append(indexes[i]);
					break;
				}
			}

			/*no index match with cgi*/
			if (i == indexes.size())
			{
				std::cout << "no index match or exist with cgi-s\n";
				me->_codeStatus = 403;
				return ;
			}
			else
			{
				// me->_finalPath.append(indexes[i]);
				/* run cgi*/
				std::cout << "-----\ni will run cgi\n"; 
				std::cout << "script file: " << me->_finalPath << "|\n";
				std::cout << "program: " << me->_cgi.cgiKeyProgram << "|\n-----\n";
				me->_cgi.executeCgi();

				exit(0);
			}

		}
		else
		{
			std::cout << "you requested a directory and did not provide any cgi  index \n";
			me->_codeStatus = 403;
			return ;
		}
	}
	else
	{
		if (me->_configData->allLocations[me->_locationKey].index.empty() == 0)
		{
			std::vector<std::string> &indexes =  me->_configData->allLocations[me->_locationKey].index;
			size_t i = 0;
			for (; i < indexes.size(); ++i)
			{
				if (me->isPathExist(std::string(me->_finalPath).append(indexes[i])))
				{
					me->_finalPath.append(indexes[i]);
					break;
				}
			}

			/*no index match with cgi*/
			if (i == indexes.size())
			{
				std::cout << "no index match --> 404\n";
				me->_codeStatus = 404;
			}
			else
			{
				std::cout << "here is your file\n";
				inputFile.open(me->_finalPath.c_str());
				if (inputFile.is_open() == false)
				{
					me->_codeStatus = 403;// it exist and did not open
				}
				else
				{
					me->_codeStatus = 200;
					generate200Header();
					me->_flags.canReadInputFile = true;
				}
			}

		}/*later auto index*/
		else if (me->_configData->allLocations[me->_locationKey].autoIndex)
		{

		}
		else
		{
			std::cout << "you requested a directory and did not provide any index \n";
			me->_codeStatus = 403;
			return ;
		}
	}


	return;
}

void	Client::Response::getMethodeResponseFile()
{
	if (me->_configData->allLocations[me->_locationKey].cgi.empty() == false)
	{
		if (me->isMatchedWithCgi(me->_finalPath)) /* if file does not exist le the child process quite with error*/
		{
			/*run cgi */
			std::cout << "-----\nget: file i will run cgi\n"; 
			std::cout << "script file: " << me->_finalPath << "|\n";
			std::cout << "program: " << me->_cgi.cgiKeyProgram << "|\n";
		}
		else
		{
			/*no cgi can run this*/
			std::cout << "get:you requested a file and did not matched with any cgi\n";
			me->_codeStatus = 403;
		}
	}
	else
	{
		/*send file*/
		inputFile.open(me->_finalPath.c_str());
		if (inputFile.is_open() == false)
		{
			me->_codeStatus = 403;// it exist and did not open
		}
		else
		{
			me->_codeStatus = 200;
			me->_flags.canReadInputFile = true;
		}
	}

	return ;
}



void	Client::Response::getMethodResponse()
{
	if (me->_flags.canReadInputFile)
		sendFileToFinalAnswer();
	else if (me->_flags.isCgiRunning)
	{
		me->_cgi.checkCgiTimeout();
	}
	else
	{
		struct stat sb;
		if (stat(me->_finalPath.c_str(), &sb) == 0)
		{
			if (S_ISDIR(sb.st_mode))
			{
				getMethodeResponseDirectory();
			}
			else if (S_ISREG(sb.st_mode))
				getMethodeResponseFile();
			else
				me->_codeStatus = 403;
		}
		else
		{
			std::cout << "get: what you requested is not found\n";
			me->_codeStatus = 404;
		}
	}
}

