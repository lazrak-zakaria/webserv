#include <bits/stdc++.h>
using namespace std;


void    split(std::string &s, std::string del, std::vector<std::string> &vec, bool flag)
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

// void    save_headers(std::string &s)
// {
//     std::vector<std::string> vec;
//     split(s, "\r\n", vec, 0);
//     for (int i = 0; i < vec.size(); ++i)
//     {
//         std::vector<std::string> line;
//         split(vec[i], " ", line, 0);
//         if (i != 0)
//             utils::str_to_lower(line[0]);
//         headers[line[0]].assign(line.begin()+1, line.end()); // later check if copy
//     }
// }


void    str_to_lower(std::string &a)
{
    for (int i = 0; i < a.length(); ++i)
        a[i] = tolower(a[i]);
}

typedef struct uri
{
    std::string methode;
    std::string http_version;
    std::string path;
    std::string query_string;
    //std::string fragment_identifier; //i see no use;
} uri;

void    parse_uri(std::map<std::string, std::vector<std::string> > headers, uri &u)
{
    std::string met = headers.count("GET") ? "GET" :  headers.count("DELETE") ? "POST" 
                            : headers.count("DELETE") ? "DELETE" : "";
    if (met.empty())
        return ;
    std::vector<std::string> &vec = headers[met];
    std::map<std::string, std::vector<std::string> >::iterator it = headers.find(met);
    //make use size is 2;
    u.methode = it->first;
    u.http_version = it->second[2];
    size_t pos = it->second[1].find('?');
    if (pos != std::string::npos)
    {
        u.path = it->second[1].substr(0, pos);
        size_t pos_fragement = it->second[1].find('#', pos);
        u.query_string = it->second[1].substr(pos+1, pos_fragement);
    }
}

void    parse_header(std::string s)
{
    size_t      separator = s.find("\r\n\r\n");
    std::string head = s.substr(0, separator + 2);
    uri u;
    std::vector<std::string> vec;
    split(head, "\r\n", vec, 0);
    // for (auto &j : vec)
    // {
    //     std::cout << j << '\n';
    // }

    std::cout << "\n";
    std::map<std::string, std::vector<std::string> > headers;
    for (int i = 0; i < vec.size(); ++i)
    {
        std::vector<std::string> line;
        split(vec[i], " ", line, 0);
        if (i != 0)
            str_to_lower(line[0]);
        headers[line[0]].assign(line.begin()+1, line.end()); // later check if copy
        if (!i)
        {
            //
            // i suppose alwaays valid methode;
            parse_uri(headers, u);
        }
    }

    for (auto &g : headers)
    {
        std::cout << g.first << "\t\t";
        for (auto &j : g.second)
        {
            std::cout << j << '\t';
        }
        std::cout << "\n";
    }
}

int main()
{
    std::string s = "GET /example/path?param1=value1&param2=value2 HTTP/1.1\r\n"
"Host: www.example.com\r\n"
"User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64; rv:89.0) Gecko/20100101 Firefox/89.0\r\n"
"Accept: text/html,application/xhtml+xml\r\n"
"Referer: https://www.google.com\r\n"
"Accept-Language: en-US,en;q=0.5\r\n"
"Connection: keep-alive\r\n\r\n"
"bodyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyy\r\n"
"ddddddddddddddddddddddddddd\r\n"
"dddddddddddddddddddddddddddd\r\n"
"ddddddddddddddddddddddddddddddddd\r\n";
    parse_header(s);
}