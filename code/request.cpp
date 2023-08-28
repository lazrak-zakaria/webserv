#include "request.hpp"
#include "utils.hpp"


void    request::split(std::string &s, std::string del, std::vector<std::string> &vec, bool flag)
{
    int start = 0, end = 0;
    bool    flag__ = 0;
    while (end != std::string::npos && end != s.size())
    {
        end = s.find(del, start);
        if (end == std::string::npos || (flag && flag__))
            end = s.size();
        
        std::string d = s.substr(start, end - start);
        if (!d.empty())
            vec.push_back(d);
        start = end + del.size();
        flag__ = 1;
    }
}

void    request::parse_header(std::string s)
{
    std::cout << s << "\n";
    std::vector<std::string>    split_part1; 
    split(s, "\r\n\r\n", split_part1, 1);

     int i = 0;
    //std::cout << split_part1.size() << "\n";
   //while (i < split_part1.size())
    //std::cout << "***********************************++++++++*\n" << split_part1[i++] << "\n***************++++++++**\n";
    //if (split_part1.size() > 1)
      //  std::cout << "++++++++++++++++\n" << split_part1.size() << "\n" << split_part1[1] << "\n++++++++++++++++++\n";

  //  std::cout << "&&&&&&&&&&&&&&&&&&&&&&&&&&  " << split_part1.size() << "\n" ;

    // std::cout << "+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n";
    // save_headers(split_part1[0]);
    // for (auto &i : this->headers)
    // {
    //     std::cout << i.first << " ";
    //     for (auto &j : i.second)
    //         std::cout << j << "\t\t";
    //     std::cout << "\n";
    // }
    // std::cout << "+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n";
    // headers.clear();
}

// https://stackoverflow.com/questions/50191786/are-http-methods-case-sensitive    methode are case sensitive;
// https://stackoverflow.com/questions/5258977/are-http-headers-case-sensitive  ==   utils::str_to_lower(line[0]);

void    request::save_headers(std::string &s)
{
    std::vector<std::string> vec;
    split(s, "\r\n", vec, 0);
    for (int i = 0; i < vec.size(); ++i)
    {
        std::vector<std::string> line;
        split(vec[i], " ", line, 0);
        if (i != 0)
            utils::str_to_lower(line[0]);
        headers[line[0]].assign(line.begin()+1, line.end()); // later check if copy
    }
}

void    request::parse_body()
{
    
}

void    request::parse_body_chunked()
{
    
}
