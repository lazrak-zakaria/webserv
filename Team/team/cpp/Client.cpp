#include "../hpp/Client.hpp"


bool		Client::isLocationMatched(const std::string &locationDirective, const std::string &path)
{
	size_t	locationLength = locationDirective.length();

	bool	pathMatchingLocation = strncmp(path.c_str(), locationDirective.c_str(), locationLength) == 0;
	bool	completeMatching = !path[locationLength];
	bool	pathHaveSlash = path[locationLength - 1] == '/'; 

	return (pathMatchingLocation && (completeMatching || pathHaveSlash));
}


void	Client::detectFinalLocation(void)
{
	std::vector<std::string> 					locationSet;
	std::map<std::string, location>::iterator	it;

	for (it = _configData->allLocations.begin(); it != _configData->allLocations.end(); ++it)
	{
		if (isLocationMatched(it->first, _request.path))
			locationSet.push_back(it->first);
	}

	if (locationSet.empty())
	{
		_codeStatus = 404;
		return;
	}

	size_t	finalLocation = locationSet[0].size();
	size_t	finalLocationIndex = 0;

	for (size_t i = 1; i < locationSet.size(); ++i)
	{
		if (locationSet[i].size() > finalLocation)
		{
			finalLocation = locationSet[i].size();
			finalLocationIndex = i;
		}
	}

	_locationKey = locationSet[finalLocationIndex];

	if (!_configData->allLocations[_locationKey].root.empty())
	{
		_finalPath = _configData->allLocations[_locationKey].root;
		if (_finalPath[_finalPath.size() - 1] != '/' && _request.path.size() > 0 && _request.path[0] != '/')
			_finalPath += '/';
		_finalPath += _request.path;
	}
	else if (!_configData->allLocations[_locationKey].alias.empty())
	{
		std::string tmp = _request.path.substr(_locationKey.length());
		_finalPath = _configData->allLocations[_locationKey].alias;
		if (_finalPath[_finalPath.size() - 1] != '/' && tmp.size() > 0 && tmp[0] != '/')
			_finalPath.push_back('/');
		_finalPath.append(tmp);
	}
	else
		_finalPath = "./default/index.html";
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
