#ifndef MIME_HPP__
#define MIME_HPP__

#include "headers.hpp"

class MimeAndError
{
	public:
		std::map<std::string, std::string> mime;
		std::map<std::string, std::string> mimeReverse;
		std::map<int, std::string> statusCode;
		std::map<int, std::string> errors;

		MimeAndError();
};

#endif