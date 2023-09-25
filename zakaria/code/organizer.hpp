


#ifndef ORGANIZER_HPP__
#define ORGANIZER_HPP__

#include "headers.hpp"

class organizer 
{
    private:
        std::vector<server>                             servers;
        std::unordered_map<std::string, std::string>    mime_mp;
};

#endif