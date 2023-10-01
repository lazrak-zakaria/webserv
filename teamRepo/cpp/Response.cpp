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
	}

	if ((howMuchRead != BUF_SIZE && howMuchRead) || !howMuchRead)
		me->_flags.isResponseFinished = true; /*make sure this flag is only here*/
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

				// exit(0);
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
	if (me->_flags.canReadInputFile && !me->_flags.isCgiFinished)
		sendFileToFinalAnswer();
	else if (me->_flags.isCgiRunning || me->_flags.isCgiFinished)
	{


		if (me->_flags.isCgiRunning)
		{
			me->_cgi.checkCgiTimeout();
		}
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
	{

		Client::Response::sendFileToFinalAnswer();
	}
	// else
	// 	generateResponseErrorHeader();
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
			exit(5);
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


// void	Client::Response::getMethodeResponseDirectory()
// {
// 	me->addSlashToFinalPath();
// 	if (me->_configData->allLocations[me->_locationKey].cgi.empty() == 0)
// 	{
// 		if (me->_configData->allLocations[me->_locationKey].index.empty() == 0)
// 		{
// 			std::vector<std::string> &indexes =  me->_configData->allLocations[me->_locationKey].index;
// 			size_t i = 0;
// 			for (; i < indexes.size(); ++i)
// 			{
// 				if (me->isMatchedWithCgi(indexes[i]) && me->isPathExist(std::string(me->_finalPath).append(indexes[i])))
// 				{
// 					me->_finalPath.append(indexes[i]);
// 					break;
// 				}
// 			}

// 			/*no index match with cgi*/
// 			if (i == indexes.size())
// 			{
// 				std::cout << "no index match or exist with cgi-s\n";
// 				me->_codeStatus = 403;
// 				return ;
// 			}
// 			else
// 			{
// 				// me->_finalPath.append(indexes[i]);
// 				/* run cgi*/
// 				std::cout << "-----\ni will run cgi\n"; 
// 				std::cout << "script file: " << me->_finalPath << "|\n";
// 				std::cout << "program: " << me->_cgi.cgiKeyProgram << "|\n-----\n";
// 				me->_cgi.executeCgi();

// 				exit(0);
// 			}

// 		}
// 		else
// 		{
// 			std::cout << "you requested a directory and did not provide any cgi  index \n";
// 			me->_codeStatus = 403;
// 			return ;
// 		}
// 	}
// 	else
// 	{
// 		if (me->_configData->allLocations[me->_locationKey].index.empty() == 0)
// 		{
// 			std::vector<std::string> &indexes =  me->_configData->allLocations[me->_locationKey].index;
// 			size_t i = 0;
// 			for (; i < indexes.size(); ++i)
// 			{
// 				if (me->isPathExist(std::string(me->_finalPath).append(indexes[i])))
// 				{
// 					me->_finalPath.append(indexes[i]);
// 					break;
// 				}
// 			}

// 			/*no index match with cgi*/
// 			if (i == indexes.size())
// 			{
// 				std::cout << "no index match --> 404\n";
// 				me->_codeStatus = 404;
// 			}
// 			else
// 			{
// 				std::cout << "here is your file\n";
// 				inputFile.open(me->_finalPath.c_str());
// 				if (inputFile.is_open() == false)
// 				{
// 					me->_codeStatus = 403;// it exist and did not open
// 				}
// 				else
// 				{
// 					me->_codeStatus = 200;
// 					me->_flags.canReadInputFile = true;
// 				}
// 			}

// 		}/*later auto index*/
// 		else if (me->_configData->allLocations[me->_locationKey].autoIndex)
// 		{

// 		}
// 		else
// 		{
// 			std::cout << "you requested a directory and did not provide any index \n";
// 			me->_codeStatus = 403;
// 			return ;
// 		}
// 	}


// 	return;
// }

// void	Client::Response::getMethodeResponseFile()
// {
// 	if (me->_configData->allLocations[me->_locationKey].cgi.empty() == false)
// 	{
// 		if (me->isMatchedWithCgi(me->_finalPath)) /* if file does not exist le the child process quite with error*/
// 		{
// 			/*run cgi */
// 			std::cout << "-----\nget: file i will run cgi\n"; 
// 			std::cout << "script file: " << me->_finalPath << "|\n";
// 			std::cout << "program: " << me->_cgi.cgiKeyProgram << "|\n";
// 		}
// 		else
// 		{
// 			/*no cgi can run this*/
// 			std::cout << "get:you requested a file and did not matched with any cgi\n";
// 			me->_codeStatus = 403;
// 		}
// 	}
// 	else
// 	{
// 		/*send file*/
// 		inputFile.open(me->_finalPath.c_str());
// 		if (inputFile.is_open() == false)
// 		{
// 			me->_codeStatus = 403;// it exist and did not open
// 		}
// 		else
// 		{
// 			me->_codeStatus = 200;
// 			me->_flags.canReadInputFile = true;
// 		}
// 	}

// 	return ;
// }



// void	Client::Response::getMethodResponse()
// {
// 	if (me->_flags.canReadInputFile)
// 		sendFileToFinalAnswer();
// 	else if (me->_flags.isCgiRunning)
// 	{
// 		me->_cgi.checkCgiTimeout();
// 	}
// 	else
// 	{
// 		struct stat sb;
// 		if (stat(me->_finalPath.c_str(), &sb) == 0)
// 		{
// 			if (S_ISDIR(sb.st_mode))
// 			{
// 				getMethodeResponseDirectory();
// 			}
// 			else if (S_ISREG(sb.st_mode))
// 				getMethodeResponseFile();
// 			else
// 				me->_codeStatus = 403;
// 		}
// 		else
// 		{
// 			std::cout << "get: what you requested is not found\n";
// 			me->_codeStatus = 404;
// 		}
// 	}
// }
/*****************************************************************/
/*****************************************************************/
/*****************************************************************/
/*****************************************************************/
/*****************************************************************/
/*****************************************************************/
/*****************************************************************/
/*****************************************************************/



// ---------------------------  GET --------------------------------//



void Client::Response::GetMethodResponse()
{
	int st;
	if (!this->me->_configData->allLocations[this->me->_locationKey].redirection.empty())
	{
		// response 301;
		std::cout << "redirect to 301 from config file  -->  " << this->me->_configData->allLocations[this->me->_locationKey].redirection << std::endl;
	}
	st = stat(this->me->_finalPath.c_str(), &this->me->_st);
	if (st < 0)
	{
		perror("Error Stat Fail : ");
		this->me->_codeStatus = 404;
		return;
	}
	else if(S_ISDIR(this->me->_st.st_mode))
	{
		this->GetDirectory();
	}
	else if(S_ISREG(this->me->_st.st_mode))
	{
		this->GetFile();
	}
}

void Client::Response::GetDirectory()
{
	std::string html;
	std::vector<std::string>::iterator Iit = this->me->_configData->allLocations[this->me->_locationKey].index.begin();
	struct stat st;
	int Ret_St;

	if (this->me->_finalPath.size() > 1 &&  this->me->_finalPath[this->me->_finalPath.size() - 1] != '/')
	{
		this->me->_codeStatus = 301;
		std::cout << "Redirect to 301 url don't have / at the end url => " << this->me->_finalPath << std::endl;
		return ;
	}
	else if(!this->me->_configData->allLocations[this->me->_locationKey].index.empty())
	{
		std::cout << "index" << std::endl;
		if (this->me->_flags.CanReadInputDir == 0)
		{
			this->me->_FdDirectory = opendir(this->me->_finalPath.c_str())	;
			if (this->me->_FdDirectory == NULL)
			{
				perror("Error opendir Fail: ");
				this->me->_codeStatus = 404;
				return ;
			}
			this->me->_ReadDirectory = readdir(this->me->_FdDirectory);
			if (this->me->_ReadDirectory == NULL)
			{
				perror("Error readdir Fail: ");
				this->me->_codeStatus = 403;
				return ;
			}
			this->me->_flags.CanReadInputDir = true;
		}
		if (this->me->_flags.CanReadInputDir)
		{
			while(Iit != this->me->_configData->allLocations[this->me->_locationKey].index.end())
			{
				Ret_St = stat((this->me->_finalPath + *Iit).c_str() , &st);
				if (Ret_St != -1)
				{
					if (this->me->isMatchedWithCgi(*Iit))
					{
						std::cout << "------------cgi---------" << std::endl;
						this->me->_cgi.executeCgi();
						return ;
					}
					else
					{
						if (this->me->_flags.canReadInputFile)
						{
							this->sendFileToFinalAnswer();
							return ;
						}
						else
						{
							this->inputFile.open(this->me->_finalPath + *Iit , std::ios::binary);
							if (!this->inputFile.is_open())
							{
								perror("Error open Fail: ");
								this->me->_codeStatus = 404;
								return;
							}
							this->sendFileToFinalAnswer();
							return ;
						}
					}
				}
				Iit++;
			}
			this->me->_flags.CanReadInputDir = false;
		}
		//should closedir if not cgi and index to be able to open it in autoindex
		// closedir(this->me->_FdDirectory);
	}
	if (this->me->_configData->allLocations[this->me->_locationKey].autoIndex)
	{
		std::cout << "autoindex" << std::endl;
		if (this->me->_flags.CanReadInputDir)
		{
			html = this->generatehtml(this->readdirectory());
			this->me->_finalAnswer = html;
			this->SendChunkDir();
			return ;
		}
		std::vector<std::string> content;
		int i = 4;
		closedir(this->me->_FdDirectory);
    	this->me->_FdDirectory = opendir(this->me->_finalPath.c_str());
    	if (!this->me->_FdDirectory)
    	{
        	perror("Error opendir fail: ");
        	this->me->_codeStatus = 404 ;
        	return ;
    	}
		this->me->_ReadDirectory = readdir(this->me->_FdDirectory);
		if (this->me->_ReadDirectory == NULL)
		{
			perror("Error readdir Fail: ");
			this->me->_codeStatus = 403;
			return ;
		}
			this->me->_flags.CanReadInputDir = true;
			html = this->generatehtml(this->readdirectory());
			this->me->_finalAnswer = html;
			this->SendChunkDir();
		
	}
	else
	{
		this->me->_codeStatus = 403;
		std::cout << "not autoindex and index" << std::endl;
	}
}

std::vector<std::string> Client::Response::readdirectory()
{
	std::vector<std::string> content;
	int i = 4;
    while(this->me->_ReadDirectory && i >= 0)
    {
		if (this->me->_ReadDirectory && this->me->_ReadDirectory->d_name && this->me->_ReadDirectory->d_name[0])
        	content.push_back(this->me->_ReadDirectory->d_name);
		this->me->_ReadDirectory = readdir(this->me->_FdDirectory);
		i--;
    }
	if (this->me->_ReadDirectory == NULL)
		this->me->_flags.isResponseFinished = true;
    return (content);
}

std::string Client::Response::generatehtml(std::vector<std::string> dir)
{
	std::string html;
	std::vector<std::string>::iterator it = dir.begin();
	if (it != dir.end())
	{
		html += "<html><ul>";
		while (it != dir.end())
		{
			html +=  "<li><a href=\"" + *it + "\">" + *it + "</a></li><br>";
            it++;
		}
		if (this->me->_flags.isResponseFinished)
        	html += "</ul></html>";
	}
	return html;
}

void Client::Response::SendChunkDir()
{
	this->me->_finalAnswer.append("/r/n");
}

void Client::Response::GetFile()
{
	if (this->me->_flags.canReadInputFile)
	{

	}
	this->inputFile.open(this->me->_finalPath, std::ios::binary);
	if (!this->inputFile.is_open())
	{
		perror("Error Open Fail: ");
		this->me->_codeStatus = 404;
		return ;
	}
	this->me->_codeStatus = 200;
	this->me->_flags.canReadInputFile = true;
	this->sendFileToFinalAnswer();
}


//---------------------------- DELETE ------------------------//

std::vector<std::string> Client::Response::DelReadDir(std::string path)
{
	std::vector<std::string> dir;
	DIR *Fd_dir = opendir(path.c_str());
	if (!Fd_dir)
	{
		perror("Error opendir fail: ");
		this->me->_codeStatus = 404;
		return dir;
	}
	struct dirent *ReadDir;
	ReadDir = readdir(Fd_dir);
	if (ReadDir == NULL)
	{
		perror("Error readdir Fail: ");
		this->me->_codeStatus = 403;
		return dir;
	}
	while(ReadDir != NULL)
	{
		dir.push_back(ReadDir->d_name);
		ReadDir = readdir(Fd_dir);
	}
	return dir;
}

void Client::Response::DeleteMethodResponse()
{
	this->delflag = 0;
	int st = stat(this->me->_finalPath.c_str(), &this->me->_st);
	if (st < 0)
	{
		perror("Error Stat Fail: ");
		this->me->_codeStatus = 404;
		return;
	}
	else if (S_ISREG(this->me->_st.st_mode))
	{
		if (this->me->_st.st_mode & S_IWUSR)
		{
			if (unlink(this->me->_finalPath.c_str()) == 0)
			{
				this->me->_codeStatus = 204;
				std::cout << "204 no content" << std::endl;
				return;
			}
			else
			{
				perror("Error Unlink Fail: ");
				this->me->_codeStatus =  500;
				return ;
			}
		}
		else
		{
			this->me->_codeStatus = 403;
			std::cout << "403 permission" << std::endl;
		}
	}
	else if (S_ISDIR(this->me->_st.st_mode))
	{
		if (this->me->_finalPath[this->me->_finalPath.size() - 1] != '/')
		{
			this->me->_codeStatus = 409;
			std::cout << "409 conflict" << std::endl;
		}
		else if(this->me->_st.st_mode & S_IWUSR)
		{
			if (this->deletedir(this->me->_finalPath) == 0)
			{
				if (rmdir(this->me->_finalPath.c_str()) == -1)
				{
					std::cout << this->me->_finalPath.c_str() << std::endl;
					perror("Error Rmdir Fail: ");
					this->me->_codeStatus = 500;
					return ;
				}
				else
				{
					me->_codeStatus = 200;
					generate200Header();
					me->_flags.canReadInputFile = true;
				}
			}
		}
		else
		{
			this->me->_codeStatus = 403;
			std::cout << "403 permission" << std::endl;
		}
	}
}

int Client::Response::deletedir(std::string path)
{
	std::vector<std::string> dir = this->DelReadDir(path);
	std::vector<std::string>::iterator it = dir.begin();
	struct stat st;
	int st_ret;
	if (this->delflag == 1)
		return 1;
	if (it->empty())
		std::cout << "/////  " <<it->empty() << std::endl;
	while(it != dir.end())
	{
		if(*it == "." || *it == "..")
		{
			it++;
			continue;
		}
		*it = path + "/" + *it;
		st_ret = stat(it->c_str(), &st);
		if(st_ret == -1)
		{
			perror("Error Stat Fail: ");
			this->delflag = 1;
			return 1;
		}
		else if (S_ISREG(st.st_mode))
		{
			if (st.st_mode & S_IWUSR)
			{
				if (unlink(it->c_str()) == -1)
				{
					perror("Error Unlink Fail: ");
					this->me->_codeStatus = 500;
					this->delflag = 1;
					return 1;
				}
				std::cout << "delete file ==> " << it->c_str() << std::endl;
			}
			else
			{
				perror("Error Permission: ");
				this->delflag = 1;
				return 1;
			}
		}
		else if (S_ISDIR(st.st_mode))
		{
			if (st.st_mode & S_IWUSR)
			{
				this->deletedir(*it);
				if (rmdir(it->c_str()) != 0)
				{
					perror("Error rmdir Fail: ");
					this->me->_codeStatus = 500;
					return 1;
				}
			}
			else
			{
				perror("Error Permission: ");
				this->delflag = 1;
				return 1;
			}
		}
		it++;
	}
	return 0;
}
