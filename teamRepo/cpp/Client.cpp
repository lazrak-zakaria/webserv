#include "../hpp/Client.hpp"


Client::Client() : _configData(NULL), _mimeError(NULL) , 
	_codeStatus(0)

{
	_request.me = this;
	_response.me = this;

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


void	Client::addSlashToFinalPath()
{
	if (_finalPath[_finalPath.size() - 1] != '/')
		_finalPath.push_back('/');
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
	// if (_request.method == "POST")
	// 	_response.postMethodeResponse();
	return _finalAnswer;
}




