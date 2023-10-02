// 201 400 403 404 405 408 409 411 413 414 500 501 502 504 505

#include "../hpp/Client.hpp"

std::string Client::Response::FindFileToOpen()
{
    struct stat st;
    int code = 0, ret_st = 0;


    if (!this->me->_configData->errorPages.empty() && !this->me->_configData->errorPages[this->me->_codeStatus].empty())
    {
        ret_st = stat(this->me->_configData->errorPages[this->me->_codeStatus].c_str(), &st);
        if (ret_st == 0)
        {
            this->inputFile.open(this->me->_configData->errorPages[this->me->_codeStatus] , std::ios::binary);
            if (this->inputFile.is_open())
            {
                return this->me->_configData->errorPages[this->me->_codeStatus];
            }
            else
                code = 403;
        }
        else
            code = 404;
    }
    if (code != 0 && !this->me->_configData->errorPages.empty() && !this->me->_mimeError->errors[code].empty())
    {
        ret_st = stat(this->me->_mimeError->errors[code].c_str(), &st);
        if (ret_st == 0)
        {
            this->inputFile.open(this->me->_mimeError->errors[code] , std::ios::binary);
            if (this->inputFile.is_open())
            {
                return this->me->_mimeError->errors[code];
            }
            else
                return "";
        }
        else
            return "";
    }
    return "";
}


void Client::Response::ErrorResponse()
{
    time_t date = time(NULL);
    std::string opnedfile;
    if (this->me->_flags.canReadInputFile) // can search for file > send chunks 
    {
        this->sendFileToFinalAnswer();
    }
    else
    {
        std::string respo(this->me->_mimeError->statusCode[this->me->_codeStatus] + "\r\n");
        respo += "Content-type: " + this->getContentTypeOfFile(opnedfile) + "\r\n";
        respo += "Content-Transfer: Chunked\r\n";
        respo += std::string("Date: ") + ctime(&date) + "\r\n";
        if (me->_request.requestHeadersMap.count("connection"))
		{
			std::string &tmp = * (--(me->_request.requestHeadersMap["connection"].end()));
			for (int i = 0; tmp[i] ; ++i)
				tmp[i] = tolower(tmp[i]);
			if (tmp.find("close") != std::string::npos)
			{
				respo += "Connection: close\r\n";
			}
			else
			{
				respo += "Connection: keep-alive\r\n";
			}
		}
		else
			respo += "Connection: keep-alive\r\n";
        this->me->_finalAnswer = respo + "\r\n";
        opnedfile = FindFileToOpen();
        if(opnedfile.empty())
        {
            this->me->_flags.isResponseFinished = true;
            this->me->_flags.canReadInputFile = false;
            std::string tmp("<html><h1>Wach Mamragtich</html></h1>\r\n");
            this->me->_finalAnswer += "\r\n" + convertToHex(tmp.size()) + tmp.append("\r\n");
        }
        else
            this->me->_flags.canReadInputFile = true;
    }
}