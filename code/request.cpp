#include "request.hpp"
#include "utils.hpp"


void    request::split(std::string &s, std::string del, std::vector<std::string> &vec)
{
    int start = 0, end = 0;
    while (end != std::string::npos)
    {
        end = s.find(del, start);
        if (end != std::string::npos)
        {
            std::string d = s.substr(start, end - start);
            if (!d.empty())
                vec.push_back(d);
            start = end + del.size();
        }
    }
}

void    request::parse(std::string s)
{
    split(s, "\r\n\r\n", split_part1);
    //save_headers(split_part1[0]);
    std::cout << "-------------\n" << split_part1[0] << "\n----------------\n";
    if (split_part1.size() > 1);
        std::cout << "-------------\n" << split_part1[1] << "\n----------------\n";
}

// https://stackoverflow.com/questions/50191786/are-http-methods-case-sensitive    methode are case sensitive;
// https://stackoverflow.com/questions/5258977/are-http-headers-case-sensitive  ==   utils::str_to_lower(line[0]);

void    request::save_headers(std::string &s)
{
    std::vector<std::string> vec;
    split(s, "\r\n", vec);
    for (int i = 0; i < vec.size(); ++i)
    {
        std::vector<std::string> line;
        split(vec[i], " ", line);
        if (i != 0)
            utils::str_to_lower(line[0]);
        headers[line[0]].assign(line.begin()+1, line.end()); // later check if copy
    }
}

