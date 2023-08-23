#ifndef req
#define req

#include "headers.hpp"

class   request
{
    public:
        std::string method;
        std::string path;
        std::string connection;
        std::string content_type;
        std::string last_modified;
        size_t      content_length;

        std::string body;
        void    parse(std::string req);
};


#endif