#include "../hpp/Client.hpp"


Client::Client() : _configData(NULL), _mimeError(NULL) , 
	_codeStatus(0)

{
	_request.me = this;
	_response.me = this;
	_cgi.me = this;
	memset(&_flags, 0, sizeof(_flags));
	_flags.expectSizeRead = true;
}

void	Client::clearClient()
{
		_finalPath.clear();
		_locationKey.clear();
		_finalAnswer.clear();
		_codeStatus = 0;

		_request.requestClear();
}

void	Client::setConfigData(ServerConfig	*c)
{
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
		std::cout << extension << "||\n";
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

	return (completeMatching || locationHaveSlash);
}



void	Client::detectFinalLocation(void)
{
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


	if (_configData->allLocations[_locationKey].allowedMethods.count(_request.method) == 0)
	{
		_codeStatus = 405;
		return ;
	}

	if (!_configData->allLocations[_locationKey].root.empty())
	{
		_finalPath = _configData->allLocations[_locationKey].root;
		// if (_finalPath[_finalPath.size() - 1] != '/' && _request.path.size() > 0 && _request.path[0] != '/')
		// 	_finalPath += '/';
		std::cout << "__"<< _request.path << "\n";
		std::cout << "__"<< _finalPath << "\n";
		
		_finalPath += _request.path;
	}
	else if (!_configData->allLocations[_locationKey].alias.empty())
	{
		std::string tmp = _request.path.substr(_locationKey.length());

		_finalPath = _configData->allLocations[_locationKey].alias;

		_finalPath.append(tmp);
	}
	else
	{
		/*what ?*/
	}
}


void	Client::generateRandomName(std::string &name) const
{
	struct timeval	tm;
	srand(time(NULL));
	for (u_int8_t i = 0; i < 15; ++i)
	{
		gettimeofday(&tm, NULL);
		size_t k = rand() + tm.tv_usec;
		switch (k % 3)
		{
			case 0:
				name.push_back((rand() + tm.tv_usec) % 26 + 'a');
			case 1:
				name.push_back((rand() + tm.tv_usec ) % 10 + '0');
			case 2:
				name.push_back((rand() + tm.tv_sec) % 26 + 'A');
		}
	}
}

void	Client::readRequest(const char * requestData, int receivedSize)
{
	if (_flags.isRequestBody)
		_request.requestBody.append(requestData, receivedSize);
	else
		_request.requestHeader.append(requestData, receivedSize);

	// std::cout << requestData << "\n";
	_request.parseRequest();
}


std::string		&Client::serveResponse(void)
{

	_finalAnswer = "";
	if (_flags.isResponseFinished) exit (10); /*debug*/

	START:

	if (_codeStatus == 301)
	{
		std::stringstream ss;
		ss << "HTTP/1.1 301 Moved Permanently\r\n";
		ss << "Location: " << _response.location301 << "\r\n\r\n";
		return (_finalAnswer = ss.str());
	}

	if (_codeStatus != 200 && _codeStatus != 201 && _codeStatus)
	{
		if(_flags.isHeaderResponseSent)
			_response.responseError();
		else
		{
			_response.generateResponseErrorHeader();
			_flags.isHeaderResponseSent = true;
		}
	}
	else if (_request.method == "POST")
	{
		_response.postMethodeResponse();
		if (_codeStatus != 200 && _codeStatus != 201 && _codeStatus)
			goto START;
	}
	else if (_request.method == "GET")
	{
		_response.getMethodResponse();
		if (_codeStatus != 200 && _codeStatus != 301 && _codeStatus)
			goto START;
	}

	return _finalAnswer;
}




