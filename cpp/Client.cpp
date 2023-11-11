#include "../hpp/Client.hpp"


Client::Client() : _configData(NULL), _mimeError(NULL) , 
	_codeStatus(0) , isCompletelySent(true)

{
	closeMe = false;
	_request.me = this;
	_response.me = this;
	_cgi.me = this;
	memset(&_flags, 0, sizeof(_flags));
	_flags.expectSizeRead = true;
	_FdDirectory = NULL;
	_ReadDirectory = NULL;
	_allConfigsData = 0;
	_timeLastAction = time(NULL);
}

const Client& Client::operator = ( const Client &c )
{
	_configData = NULL;
	_mimeError = (NULL); 
	_codeStatus = (0);
	isCompletelySent = (true);

	closeMe = false;
	_request.me = this;
	_response.me = this;
	_cgi.me = this;
	memset(&_flags, 0, sizeof(_flags));
	_flags.expectSizeRead = true;
	_FdDirectory = NULL;
	_ReadDirectory = NULL;
	_allConfigsData = 0;
	_timeLastAction = time(NULL);
	(void)c;

	return *this;
}

Client::Client( const Client &c )
{
	if (this != &c)
		*this = c;
}

Client::~Client()
{
	if (this->_FdDirectory)
		closedir(this->_FdDirectory);
	clearClient();
}

void	Client::clearClient()
{
		_finalPath = "";
		_locationKey = "";
		_finalAnswer = "";
		_codeStatus = 0;
		if (_FdDirectory)
			closedir(_FdDirectory);
		_FdDirectory = NULL;
		_ReadDirectory = NULL;
		
		
		memset(&_flags, 0, sizeof(_flags));
		_flags.expectSizeRead = true;
		_request.requestClear();
		_response.responseClear();


		for (size_t i = 0 ; i < filesToDelete.size(); ++i)
			unlink(filesToDelete[i].c_str());
		filesToDelete.clear();
		_cgi.clearCgi();
}

void	Client::setRequestFinished(u_int16_t codeNum)
{
	_codeStatus = codeNum;
	_flags.isRequestFinished = true;
}

std::string &Client::trim(std::string& str)
{
    str.erase(str.find_last_not_of(' ')+1);
    str.erase(0, str.find_first_not_of(' '));
    return str;
}


void	Client::setAllConfigData(std::map<std::string, ServerConfig*> *d)
{
	_allConfigsData = d;
}

void	Client::setDefaultConfigData(ServerConfig	*c)
{
	_defaultConfigData = c;
	_configData = c;
}

void	Client::setMimeError(MimeAndError	*m)
{
	_mimeError = m;
}

bool	Client::isResponseFinished() const
{
	return _flags.isResponseFinished;
}

bool	Client::isRequestFinished() const
{
	return _flags.isRequestFinished;
}

bool	Client::isPathExist(std::string path)
{
	struct stat sb;
	return stat(path.c_str(), &sb) == 0;
}

void	Client::addSlashToFinalPath()
{
	if (_finalPath[_finalPath.size() - 1] != '/')
		_finalPath.push_back('/');
}

size_t	Client::getTimeNow()
{
	struct timeval tmv;
	gettimeofday(&tmv, NULL);
	return tmv.tv_sec;
}

bool	Client::isMatchedWithCgi(std::string &file)
{
	size_t pos = file.find_last_of('.');
	if (pos != std::string::npos)
	{
		std::string	extension = file.substr(pos);
		std::map<std::string, std::string>& cgi = _configData->allLocations[_locationKey].cgi;
		std::map<std::string, std::string>::iterator it;
		
		for (it = cgi.begin(); it != cgi.end(); ++it)
		{
			if (extension == it->first)
			{
				_cgi.cgiKeyProgram = extension;
				return true;
			}
		}
	}
	return 0;
}

bool		Client::isLocationMatched(const std::string &locationDirective, const std::string &path)
{
	size_t	locationLength = locationDirective.length();

	bool	pathMatchingLocation = strncmp(path.c_str(), locationDirective.c_str(), locationLength) == 0;

	bool	completeMatching = pathMatchingLocation && !path[locationLength];
	bool	locationHaveSlash = pathMatchingLocation && locationDirective[locationLength-1] == '/';
	bool	pathHaveSlash = pathMatchingLocation && path[locationLength] == '/';
	return (completeMatching || locationHaveSlash || pathHaveSlash);
}



void	Client::detectFinalLocation(void)
{

	if (!_request.requestHeadersMap.count("host"))
		return ;
	
	std::string &host = *(_request.requestHeadersMap["host"].end()-1);
	trim(host);
	if (_allConfigsData && _allConfigsData->count(host))
		_configData = _allConfigsData->at(host);
	else
		_configData = _defaultConfigData;

	std::vector<std::string> 					locationSet;
	std::map<std::string, location>::iterator	it;

	for (it = _configData->allLocations.begin(); it != _configData->allLocations.end(); ++it)
	{
		if (isLocationMatched(it->first, _request.path))
		{
			if (_locationKey.size() < it->first.size())
				_locationKey = it->first;
		}
	}

	if (_locationKey.empty())
	{
		_codeStatus = 404;
		return ;
	}

	if (!_configData->allLocations[_locationKey].root.empty())
	{
		_finalPath = _configData->allLocations[_locationKey].root;
		_finalPath += _request.path;
	}
	else if (!_configData->allLocations[_locationKey].alias.empty())
	{
		std::string tmp = _request.path.substr(_locationKey.length());
		_finalPath = _configData->allLocations[_locationKey].alias;
		_finalPath.append(tmp);
	}
	else
		_finalPath = _request.path;

}


void	Client::generateRandomName(std::string &name) const
{
	struct timeval	tm;
	srand(time(NULL));
	for (u_int8_t i = 0; i < 20; ++i)
	{
		gettimeofday(&tm, NULL);
		size_t k = rand() + tm.tv_usec;
		switch (k % 3)
		{
			case 0:
				name.push_back((rand() + tm.tv_usec) % 26 + 'a');
				break;
			case 1:
				name.push_back((rand() + tm.tv_usec ) % 10 + '0');
				break;
			case 2:
				name.push_back((rand() + tm.tv_sec) % 26 + 'A');
		}
	}
}

void	Client::readRequest(const char * requestData, int receivedSize)
{
	if (_flags.isRequestBody)
	{
		_request.requestBody.append(requestData, receivedSize);
	}
	else
	{
		_request.requestHeader.append(requestData, receivedSize);
	}
	_request.parseRequest();
}


std::string		&Client::serveResponse(void)
{

	_finalAnswer = "";

	START:
	if (_codeStatus != 200 && _codeStatus != 201  && _codeStatus != 204 && _codeStatus)
	{
		if (_codeStatus == 307)
			_response.GenerateLastResponseHeader(307, "", NULL);
		else if (_codeStatus == 301)
		{
			_finalAnswer = _mimeError->statusCode[301] + "\r\n";
			_finalAnswer += "location: " + _response.location301 + "\r\n\r\n";
			_flags.isResponseFinished = true;
		}
		else
			_response.ErrorResponse();
	}
	else if (_request.method == "POST")
	{
		_response.postMethodeResponse();
		if (_codeStatus != 200 && _codeStatus != 201 && _codeStatus)
		{
			_response.inputFile.close();
			goto START;
		}
	}
	else if (_request.method == "GET")
	{
		_response.GetMethodResponse();
		if (_codeStatus != 200 && _codeStatus != 301 && _codeStatus != 204 && _codeStatus)
		{
			_response.inputFile.close();
			goto START;
		}
	}
	else if(_request.method == "DELETE")
	{
		_response.DeleteMethodResponse();
		if (_codeStatus != 200 && _codeStatus != 301 && _codeStatus != 204 && _codeStatus)
		{
			_response.inputFile.close();
			goto START;
		}
	}
	return _finalAnswer;
}
