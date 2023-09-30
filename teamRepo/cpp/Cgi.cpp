#include "../hpp/Client.hpp"


void Client::Cgi::parseCgiHeader()
{
	const int BUFSIZE = 40000;
	char buffer[BUFSIZ];

	me->_response.inputFile.read(buffer, BUFSIZ - 2);

	int	howMuchRead = me->_response.inputFile.gcount();
	cgiHeader.append(buffer, howMuchRead);
	size_t pos = cgiHeader.find("\n\n");
	if (pos == std::string::npos)
	{
		std::cout << "didnot found the end of header\n";
		std::string().swap(cgiHeader);
		me->_codeStatus = 500;
	}
	else
	{
		cgibody = cgiHeader.substr(pos + 2);
		cgiHeader.erase(pos + 2);
		enum {eFieldName, eFieldNameChar, eFieldValue, eFieldValueChar, loopToNewLine};

		u_int8_t	cursor = eFieldNameChar;
		u_int8_t	currentState = eFieldName ;
		std::string	key, value;


		for (size_t i = 0; i < cgiHeader.size(); ++i)
		{
			char &currentChar = cgiHeader[i];
			switch (currentState)
			{
				case eFieldName:
					switch (cursor)
					{
					case eFieldNameChar:
						if (currentChar == ':' && !key.empty())
						{
							currentState = eFieldValue;
							cursor = eFieldValueChar;
						}
						else if (me->_request.isFieldNameValid(currentChar))
						{
							key.push_back(tolower(currentChar));
						}
						else
							currentState = loopToNewLine;
					}
					break;
				case eFieldValue:
					switch (cursor)
					{
						case eFieldValueChar:
							if (me->_request.isFieldValueValid(currentChar))
								value.push_back(currentChar);
							else if (currentChar == '\n')
							{
								cgiHeadersMap[key] = value;
								key.clear();
								value.clear();
								currentState = eFieldName;
								cursor = eFieldNameChar;
							}
							else
								currentState = loopToNewLine;
							break;
					}
					break;
				case loopToNewLine:
					for (; i < cgiHeader.size() && cgiHeader[i] != '\n'; ++i);
					currentState = eFieldName;
					cursor = eFieldNameChar;
					key.clear();
					value.clear();
			}
		}

		if (cgiHeadersMap.count("status"))
		{
			std::string tmp;
			size_t i = 0;
			while (cgiHeadersMap["status"][i] == ' ')
				++i;
			u_int8_t j = 0; 
			while (cgiHeadersMap["status"][i] && cgiHeadersMap["status"][i] != ' ')
			{
				if (!isdigit(cgiHeadersMap["status"][i]) || j > 2)
				{
					std::cout << "should foun only digits -----> |"<< cgiHeadersMap["status"][i] << "|\n";
					me->_codeStatus = 502;
					return ;
				}
				else
				{
					std::cout << "----------------------------->|" << cgiHeadersMap["status"][i] << "|\n";
					tmp.push_back(cgiHeadersMap["status"][i++]);
				}
				++j;
			}
			if (tmp > "599" || j != 3)
			{
				std::cout << "code max\n";
				me->_codeStatus = 502;
				return ;
			}
			statusLine = std::string("HTTP/1.1 ").append(tmp);
			tmp = "";
			if (cgiHeadersMap["status"][i++] != ' ')		
			{
				std::cout << "i should find space|" << cgiHeadersMap["status"][i] << "|\n";
				me->_codeStatus = 502;
				return ;
			}
			statusLine.push_back(' ');
			while (cgiHeadersMap["status"][i])
				statusLine.push_back(cgiHeadersMap["status"][i++]);
			statusLine.append("\r\n");
			cgiHeadersMap.erase("status");
		}
		else
			statusLine = "HTTP/1.1 200 OK\r\n";

		if (cgiHeadersMap.count("content-type") == 0)
		{
			std::cout << "i should find content type\n";
			me->_codeStatus = 502;
			return ;
		}

		std::map<std::string, std::string>::iterator it;
		std::stringstream ss;
		ss << statusLine;
		std::cout << "--------------------------------------\n";
		for (it = cgiHeadersMap.begin(); it != cgiHeadersMap.end(); ++it)
		{
			std::cout << "|" << it->first <<"| |" <<it->second<< "|\r\n"; 
			ss << it->first << ": " << it->second<< "\r\n"; 
		}
		std::cout << "--------------------------------------\n";
		
		if (cgiHeadersMap.count("server") == 0)
			ss << "server: servingThings\r\n";

		if (cgiHeadersMap.count("content-length") == 0)
			ss << "transfer-encoding: chunked\r\n";
		else
		{
			me->_flags.isCgiHaveContentLength = true;
		}

		ss << "\r\n";
		me->_finalAnswer = ss.str();
		std::cout << "________+++++\n";
	}
}

void		Client::Cgi::sendCgiBodyToFinaleAnswer()
{
	const int BUFERSIZE = 5000;
	char buf[BUFERSIZE + 2];

	if (cgibody.empty())
	{
		me->_response.inputFile.read(buf, BUFERSIZE);
		int	howMuchRead = me->_response.inputFile.gcount();
			
		if (me->_flags.isCgiHaveContentLength == false)
		{
			std::stringstream ss;
			ss << std::hex << howMuchRead;
			me->_finalAnswer = "";
			me->_finalAnswer.append(ss.str().append("\r\n"));

			buf[howMuchRead] = '\r';
			buf[howMuchRead + 1] = '\n';

			me->_finalAnswer.append(buf, howMuchRead + 2);
			
			if (howMuchRead != BUFERSIZE && howMuchRead)
			{
				me->_finalAnswer.append("0\r\n");
				me->_response.inputFile.close();
				me->_flags.isResponseFinished = true; /*make sure this flag is only here*/
			}
		}
		else
		{
			me->_finalAnswer.append(buf, howMuchRead);
			if (howMuchRead != BUFERSIZE && howMuchRead)
			{
				me->_response.inputFile.close();
				me->_flags.isResponseFinished = true; 
			}
		}

	}
	else
	{
		if (me->_flags.isCgiHaveContentLength == false)
		{
			std::stringstream ss;
			ss << std::hex << cgibody.size();
			me->_finalAnswer = "";
			me->_finalAnswer.append(ss.str().append("\r\n"));

			me->_finalAnswer.append(cgibody.append("\r\n"));
		}
		else
			me->_finalAnswer.append(cgibody);
		cgibody = "";
	}
	
}

/*even if kill the process you should wait*/
void Client::Cgi::checkCgiTimeout()
{

	if (waitpid(processPid, 0, WNOHANG) == processPid)
	{
		/*cgi end good && later check exit status*/
		me->_flags.canReadInputFile = true;
	}
	else
	{
		struct timeval tmv;
		gettimeofday(&tmv, NULL);
		size_t timeNow =  tmv.tv_sec;
		if (timeNow - timeStart > 7)
		{
			kill(processPid, SIGKILL);
			waitpid(processPid, 0, WNOHANG);
			me->_codeStatus = 504;
		}
	}
}


void	Client::Cgi::executeCgi()
{

	/*open outputfile here*/
	me->generateRandomName(outputFileCGi);
	outputFileCGi = std::string("./../tmp/TTT").append(outputFileCGi);
	std::ofstream outfile (outputFileCGi);
	if (outfile.is_open() == false)
	{
		std::cout << "FAILeD++++\n";
		me->_codeStatus = 500;
		return ;
	}
	outfile.close();
	processPid = fork();
	if (processPid == 0)
	{
		/*Setup env variables*/
		char* env[10];
		env[0] = NULL;

		me->_response.inputFile.close();

		std::string	&programName 	= me->_configData->allLocations[me->_locationKey].cgi[cgiKeyProgram];
		std::string	&scriptToexec	= me->_finalPath;
		
		if (inputFileCGi.empty() == false) /*for post*/
		{
			if (freopen(inputFileCGi.c_str(), "r", stdin) == NULL)
			{
				std::cout << "CGI INPUT FAIL\n";
				exit(1);
			}
		}
		if (freopen(outputFileCGi.c_str(), "w", stdout) == NULL)
		{
			std::cout << "CGI outPUT FAIL\n";
			exit(1);
		}
		char *argv[3];


		argv[0] = "../cgi_bin/a.out" ;//strdup(programName.c_str());
		argv[1] = strdup(scriptToexec.c_str());
		argv[2] = NULL;
		std::cout << argv[0] <<  "++" << argv[1] << "\n";
		execve(argv[0], argv, env);
		std::cout << "/*******/\n";
		exit(5);
	}
	else
	{
		struct timeval tmv;
		gettimeofday(&tmv, NULL);
		timeStart =  tmv.tv_sec;

		// int ret = waitpid(processPid, 0, WNOHANG);
		int ret = wait(0); /* to test*/
		if (ret == processPid)
		{
			me->_flags.isCgiFinished = true;
			me->_response.inputFile.open(outputFileCGi.c_str(), std::ios::binary);
			std::cout << "-------------------------------->" << outputFileCGi << '\n';
		}
		else if (ret == 0)
		{
			me->_flags.isCgiRunning = true;
			return ;
		}
		if (me->_response.inputFile.is_open() == 0)
		{
				me->_codeStatus = 500;
			std::cout << "open failed to read cgi output\n";
		}
	}
}
