#include "utils.hpp"

void utils::str_to_lower(std::string &s)
{
    for (int i = 0; s[i] ; ++i)
        s[i] = tolower(s[i]);
}