#ifndef MIME_HPP__
#define MIME_HPP__

#include "header.hpp"

class mime_and_status_code
{
	public:
	std::map<std::string, std::string> mime;
	std::map<int, std::string> status_code;

	mime_and_status_code();
};

#endif