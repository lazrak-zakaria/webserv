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

        std::map<std::string, std::vector<std::string> > headers;

       // std::vector<std::string>    split_part1; // split with "\r\n\r\n"
        std::vector<std::string>    split_part2; // split with "\r\n"

        std::string body;
        
        void    parse(std::string req);
        void    save_headers(std::string &s);
        void    split(std::string &s, std::string del, std::vector<std::string> &vec, bool f);
        void    parse_body();

};


#endif