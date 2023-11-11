#include "../hpp/Client.hpp"


Client::Response::Response()
{

}

Client::Response::Response (const Response& r)
{
	if (this != &r)
		*this = r;
}
Client::Response& Client::Response::operator = (const Response& r)
{
	responseHeader = r.responseHeader;
	chunkedSize = r.chunkedSize;
	location301 = r.location301;
	return *this;
}

Client::Response::~Response()
{

}

void	Client::Response::responseClear()
{
	responseHeader.clear();
	chunkedSize = 0;
	inputFile.close();
	location301.clear();

}


std::string		Client::Response::convertToHex(size_t decimalNum)
{
	std::stringstream ss;
	ss << std::hex << decimalNum;
	return ss.str();
}

void	Client::Response::sendFileToFinalAnswer()
{
	const int 	BUF_SIZE = 8192; 
	char		buf[BUF_SIZE + 2];
	inputFile.read(buf, BUF_SIZE);

	int	howMuchRead = inputFile.gcount();
	me->_finalAnswer = "";
	me->_finalAnswer.append(convertToHex(howMuchRead).append("\r\n"));
	buf[howMuchRead] = '\r';
	buf[howMuchRead + 1] = '\n';

	me->_finalAnswer.append(buf, howMuchRead + 2);

	if (howMuchRead != BUF_SIZE && howMuchRead)
	{
		me->_finalAnswer.append("0\r\n");
		inputFile.close();
	}

	if ((howMuchRead != BUF_SIZE && howMuchRead) || !howMuchRead)
		me->_flags.isResponseFinished = true;
}

void	Client::Response::postMethodeResponseDirectory()
{
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

			if (i == indexes.size())
			{
				std::cerr << "no index match or exist with cgi\n";
				me->_codeStatus = 404;
			}
			else
				me->_cgi.executeCgi();
		}
		else
		{
			std::cerr << "directory request need cgi-index\n";
			me->_codeStatus = 404;
		}
	}
	else
	{
		std::cerr << "post request with no [cgi or upload]\n";
		me->_codeStatus = 403;
	}
}

void	Client::Response::postMethodeResponseFile()
{
	if (!me->_configData->allLocations[me->_locationKey].cgi.empty())
	{
		if (me->isMatchedWithCgi(me->_finalPath)) 
			me->_cgi.executeCgi();
		else
		{
			std::cerr << "cgi not matched\n";
			me->_codeStatus = 403;
			return ;
		}
	}
	else
	{
		me->_codeStatus = 403;
		return ;
	}
}


void	Client::Response::postMethodeResponse()
{
	if (me->_flags.canReadInputFile && !me->_flags.isCgiFinished)
		sendFileToFinalAnswer();
	else if (me->_flags.isCgiRunning || me->_flags.isCgiFinished)
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
				me->_cgi.sendCgiBodyToFinaleAnswer();
		}
	}
	else
	{
		if (me->_codeStatus == 201)
		{
			GenerateLastResponseHeader(201, me->_request.uploadFileName, NULL);
			return ;
		}

		struct stat sb;
		if (stat(me->_finalPath.c_str(), &sb) == 0)
		{
			if (S_ISDIR(sb.st_mode))
				postMethodeResponseDirectory();
			else if (S_ISREG(sb.st_mode))
				postMethodeResponseFile();
			else
				me->_codeStatus = 403;
		}
		else
		{
			std::cerr << "request path is not found ; how you end up here anyway!!\n";
			me->_codeStatus = 404;
		}
	}
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

std::string	Client::Response::connectionHeader()
{
	if (me->_request.requestHeadersMap.count("connection"))
	{
		std::string &tmp = * (--(me->_request.requestHeadersMap["connection"].end()));
		for (int i = 0; tmp[i] ; ++i)
			tmp[i] = tolower(tmp[i]);
		if (tmp.find("close") != std::string::npos)
		{
			me->closeMe = true;
			return std::string("Connection: close\r\n");
		}
		else
			return std::string("Connection: keep-alive\r\n");
	}
	else
		return std::string("Connection: keep-alive\r\n");
}

std::string Client::Response::FindFileToOpen()
{
    struct stat st;
    int code = 0, ret_st = 0;

    if (!this->me->_configData->errorPages.empty() && 
		this->me->_configData->errorPages.count(this->me->_codeStatus) &&  !this->me->_configData->errorPages[this->me->_codeStatus].empty())
    {
        ret_st = stat(this->me->_configData->errorPages[this->me->_codeStatus].c_str(), &st);
        if (ret_st == 0)
        {
            this->inputFile.open(this->me->_configData->errorPages[this->me->_codeStatus].c_str() , std::ios::binary);
            if (this->inputFile.is_open())
            {
                return this->me->_configData->errorPages[this->me->_codeStatus];
            }
            else
                code = 403;
        }
        else
		{
            code =  404;
		}
    }
    if (code == 0 && !this->me->_mimeError->errors.empty() && !this->me->_mimeError->errors[this->me->_codeStatus].empty())
    {
        ret_st = stat(this->me->_mimeError->errors[this->me->_codeStatus].c_str(), &st);
        if (ret_st == 0)
        {
            this->inputFile.open(this->me->_mimeError->errors[this->me->_codeStatus].c_str() , std::ios::binary);
            if (this->inputFile.is_open())
            {
                return this->me->_mimeError->errors[this->me->_codeStatus];
            }
            else
                return "";
        }
        else
		{
            return "";
		}
    }
	if (code != 0 && !this->me->_mimeError->errors.empty() && !this->me->_mimeError->errors[code].empty())
    {
        ret_st = stat(this->me->_mimeError->errors[code].c_str(), &st);
        if (ret_st == 0)
        {
            this->inputFile.open(this->me->_mimeError->errors[code].c_str() , std::ios::binary);
            if (this->inputFile.is_open())
            {
				this->me->_codeStatus = code;
                return this->me->_mimeError->errors[code];
            }
            else
                return "";
        }
        else
		{
            return "";
		}
    }
    return "";
}


void Client::Response::ErrorResponse()
{
    time_t date = time(NULL);
    std::string opnedfile;
    if (this->me->_flags.canReadInputFile)
    {
        this->sendFileToFinalAnswer();
    }
    else
    {
		opnedfile = FindFileToOpen();

        std::string respo(this->me->_mimeError->statusCode[this->me->_codeStatus] + "\r\n");
		if (!opnedfile.empty())
        	respo += "Content-type: " + this->getContentTypeOfFile(opnedfile) + "\r\n";
		if (me->_codeStatus == 201)
			respo += "location: " +  this->me->_request.path + me->_request.uploadFileName + "\r\n";
        respo += "Transfer-encoding: chunked\r\n";
		
        respo += std::string("Date: ") + ctime(&date);
		respo.erase(--respo.end());
		respo += "\r\n";
		respo += connectionHeader();
        this->me->_finalAnswer = respo + "\r\n";
		this->me->_flags.isHeaderResponseSent = true;

        if(opnedfile.empty())
        {
            this->me->_flags.isResponseFinished = true;
            this->me->_flags.canReadInputFile = false;
            std::string tmp = "<h1>SURPRISE !!</h1>";
            this->me->_finalAnswer.append(convertToHex(tmp.size()).append("\r\n"));
			this->me->_finalAnswer += tmp.append("\r\n0\r\n");
		}
        else
        	this->me->_flags.canReadInputFile = true;
    }
}

void Client::Response::GenerateLastResponseHeader(int status, std::string filename, struct stat *st)
{
	std::string respo (this->me->_mimeError->statusCode[status]);
	respo.append("\r\n");

	switch (status)
	{
		case 307:
		case 301:
			respo += "location: " + this->me->_request.path + "/\r\n";
			this->me->_flags.isResponseFinished = true;
			break;
		case 201:
			ErrorResponse();
			return ;
		case 204:
		case 200:
			if (!filename.empty())
			{
				respo += "content-type: " + this->getContentTypeOfFile(filename) + "\r\n";
			}
			respo += "transfer-encoding: chunked\r\n";
			time_t date = time(NULL);
			respo += std::string("Date: ") + ctime(&date);
			respo.erase(--respo.end());
			respo += "\r\n";
			if (!filename.empty() && st)
			{
				respo += std::string("Last-Modified: ") + ctime(&st->st_mtime);
				respo.erase(--respo.end());
				respo += "\r\n";
			}
			respo += connectionHeader();
	}
	this->me->_finalAnswer = respo + "\r\n";

}


// ---------------------------  GET --------------------------------//

void Client::Response::GetMethodResponse()
{

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
				me->_cgi.sendCgiBodyToFinaleAnswer();
		}
		return ;
	}
	int st;
	if (!this->me->_configData->allLocations[this->me->_locationKey].redirection.empty())
		this->GenerateLastResponseHeader(301, "", NULL);
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

	if (this->me->_request.path[this->me->_request.path.size() - 1] != '/')
	{
		this->me->_codeStatus = 301;
		this->GenerateLastResponseHeader(301, "", NULL);
		return ;
	}
	else if(!this->me->_configData->allLocations[this->me->_locationKey].index.empty())
	{
		if (this->me->_flags.canReadInputFile)
		{
			this->sendFileToFinalAnswer();
			return ;
		}
		else
		{
			while(Iit != this->me->_configData->allLocations[this->me->_locationKey].index.end())
			{
				Ret_St = stat((this->me->_finalPath + *Iit).c_str() , &st);
				if (Ret_St != -1)
				{
					if (this->me->isMatchedWithCgi(*Iit))
					{
						me->_finalPath.append(*Iit);
						this->me->_cgi.executeCgi();
						return ;
					}
					else
					{
						this->inputFile.open((this->me->_finalPath + *Iit).c_str(), std::ios::binary);
						if (!this->inputFile.is_open())
						{
							perror("Error open Fail: ");
							this->me->_codeStatus = 403;
							return;
						}
						this->GenerateLastResponseHeader(200, *Iit, &st);
						this->me->_flags.canReadInputFile = true;
						return ;
					}
				}
				Iit++;
			}
		}
	}
	if (this->me->_configData->allLocations[this->me->_locationKey].autoIndex)
	{
		if (this->me->_flags.CanReadInputDir)
		{
			html = this->generatehtml(this->readdirectory());
			this->me->_finalAnswer = this->convertToHex(html.size()).append("\r\n") + html + "\r\n";
			if (this->me->_flags.isResponseFinished)
				this->me->_finalAnswer.append("0\r\n");

			return ;
		}
		std::vector<std::string> content;

		Ret_St = stat(this->me->_finalPath.c_str(), &st);
		if (Ret_St < 0)
		{
			this->me->_codeStatus = 404;
			return;
		}
		if  (this->me->_FdDirectory)
			closedir(this->me->_FdDirectory);
    	this->me->_FdDirectory = opendir(this->me->_finalPath.c_str());
    	if (!this->me->_FdDirectory)
    	{
        	perror("Error opendir fail: ");
        	this->me->_codeStatus = 403 ;
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
		this->GenerateLastResponseHeader(200, ".html", NULL);
		html += std::string("<html><br /><h1 style=\"display:inline\">Index: </h1>") + "<h3 style=\"display:inline\">" + this->me->_request.path + "</h3>" + "<hr style=\" height:2px; background-color:black\">";
		html += "<ul>" + this->generatehtml(this->readdirectory());
		this->me->_finalAnswer += this->convertToHex(html.size()).append("\r\n") + html + "\r\n";
		if (this->me->_flags.isResponseFinished)
			this->me->_finalAnswer.append("0\r\n");

		html.clear();
	}
	else
		this->me->_codeStatus = 403;
}

std::vector<std::string> Client::Response::readdirectory()
{
	std::vector<std::string> content;
	int i = 4;
    while(this->me->_ReadDirectory && i >= 0)
    {
		if (this->me->_ReadDirectory)
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

void Client::Response::GetFile()
{
	struct stat st;

	if (this->me->_flags.canReadInputFile)
	{
		this->sendFileToFinalAnswer();
		return ;
	}
	else
	{
		int s = stat(this->me->_finalPath.c_str(), &st);
		if (s == 0 && this->me->isMatchedWithCgi(this->me->_finalPath) == true)
		{
			this->me->_cgi.executeCgi();
			return ;
		}
		if (!(st.st_mode & S_IRUSR))
		{
			this->me->_codeStatus = 403;
			return ;
		}
		this->inputFile.open(this->me->_finalPath.c_str(), std::ios::binary);
		if (!this->inputFile.is_open())
		{
			perror("Error Open Fail: ");
			this->me->_codeStatus = 404;
			return ;
		}
		this->me->_codeStatus = 200;
		this->me->_flags.canReadInputFile = true;
		struct stat st;
		stat(this->me->_finalPath.c_str(), &st);
		this->GenerateLastResponseHeader(200, this->me->_finalPath, &st);
	}
}


//---------------------------- DELETE ------------------------//

std::vector<std::string> Client::Response::DelReadDir(std::string path)
{
	std::vector<std::string> dir;
	DIR *Fd_dir = opendir(path.c_str());
	if (!Fd_dir)
	{
		perror("Error opendir fail: ");
		this->me->_codeStatus = 403;
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
	if (this->me->_flags.canReadInputFile)
	{
		this->sendFileToFinalAnswer();
		return ;
	}
	int st = stat(this->me->_finalPath.c_str(), &this->me->_st);
	if (st < 0)
	{
		perror("Error Stat Fail: ");
		this->me->_codeStatus = 404;
		return;
	}
	else if (S_ISREG(this->me->_st.st_mode))
	{
		if (unlink(this->me->_finalPath.c_str()) == 0)
		{
			this->me->_codeStatus = 204;
			this->GenerateLastResponseHeader(204, "", NULL);
			me->_flags.isResponseFinished = true;
			this->me->_finalAnswer.append("0\r\n");
			return;
		}
		else
		{
			perror("Error Unlink Fail: ");
			this->me->_codeStatus =  403;
			return ;
		}
	}
	else if (S_ISDIR(this->me->_st.st_mode))
	{
		if (this->me->_finalPath[this->me->_finalPath.size() - 1] != '/')
			this->me->_codeStatus = 409;
		if (this->deletedir(this->me->_finalPath) == 0 && this->me->_codeStatus != 403)
		{
			if (rmdir(this->me->_finalPath.c_str()))
			{
				perror("Error Rmdir Fail: ");
				this->me->_codeStatus = 403;
				return ;
			}
			else
			{
				me->_codeStatus = 204;
				GenerateLastResponseHeader(204, "", NULL);
				me->_flags.isResponseFinished = true;
				this->me->_finalAnswer.append("0\r\n");
			}
		}
		else
		{
			this->me->_codeStatus = 403;
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
			if (unlink(it->c_str()) == -1)
			{
				perror("Error Unlink Fail: ");
				this->delflag = 1;
				return 1;
			}
		}
		else if (S_ISDIR(st.st_mode))
		{
			this->deletedir(*it);
			if (rmdir(it->c_str()) != 0)
			{
				perror("Error rmdir Fail: ");
				this->delflag = 1;
				return 1;
			}
		}
		it++;
	}
	return 0;
}
