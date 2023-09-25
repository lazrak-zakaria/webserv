#include <bits/stdc++.h>
using namespace std;
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>


typedef struct location
{
    std::string root;                           //only one them root or alias
    std::string alias; //thats what was asked in the subject;
    std::vector<std::string> cgi_ext;
    std::vector<std::string> cgi_param;
    std::vector<std::string> cgi_path;
    std::unordered_set<std::string> allowed;
    bool                            auto_index;

} location;

class config
{
    public:
        std::string server_name;
        std::string port;
        std::unordered_map<std::string, location> locations;
        std::unordered_map<std::string, std::string> error_page;
};

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
    std::string code;
} uri;

void    parse_uri(std::map<std::string, std::vector<std::string> > headers, uri &u)
{
    std::map<std::string, std::vector<std::string> >::iterator it = headers.find("GET");
    if (it == headers.end())
    {
        it = headers.find("POST");
        if (it == headers.end())
            it = headers.find("DELETE");
        if (it == headers.end())
            return ;
    }
    //make use size is 2;
    u.methode = it->first;
    u.http_version = it->second[1];
    //std::cout << "http\t" << "\n";
    size_t pos = it->second[0].find('?');
    if (pos != std::string::npos)
    {
        u.path = it->second[0].substr(0, pos);
        size_t pos_fragement = it->second[0].find('#', pos);
        u.query_string = it->second[0].substr(pos+1, pos_fragement);
    }
    else
    {
        pos = it->second[0].find('#');
        u.path = it->second[0].substr(0, pos);
    }
    // std::cout << "http\t" << u.http_version << "\n";
    // std::cout << "path\t" << u.path << "\n";
    // std::cout << "query_string\t" << u.query_string << "\n";
    // std::cout << "methode\t" << u.methode << "\n";
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

    // for (auto &g : headers)
    // {
    //     std::cout << g.first << "\t\t";
    //     for (auto &j : g.second)
    //     {
    //         std::cout << j << '\t';
    //     }
    //     std::cout << "\n";
    // }
}

typedef struct response
{
    std::string response_line = "HTTP/1.1 ";
    std::string date;
    std::string server;
    std::string connection;
    std::string content_type;
    std::string last_modified;
    std::string body;
} response; 

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






// s2 == uri;
// s == location

bool    start_with(const std::string &s, std::string &s2)
{
    if (s2.substr(0, s.length()) == s)
        return 1;
    return 0;
}


std::string return_file(location *l, uri &u, bool root_alias)
{
    std::string answer = "";
    if (l)
    {
        if (root_alias) // root end with /
        {
            if (l->root[l->root.length() - 1] != '/')
                l->root += '/';
            answer =  l->root + u.path;
        }
        else
        {
            //make sure alias end with /
            if (l->alias[l->alias.length() - 1] != '/')
                l->alias += '/';
            answer = l->alias + u.path.substr(l->alias.length());
        }
    }
}

location    *return_location(config &f, uri &u)
{
    std::vector<std::string>   location_set;
    for (std::unordered_map<std::string, location>::iterator it = f.locations.begin(); it != f.locations.end(); ++it)
    {
        if (start_with(it->first, u.path))
            location_set.push_back(it->first);
    }
    if (location_set.empty()) return NULL;
    size_t mx = location_set[0].length();
    int j = 0;
    for (int i = 1; i < location_set.size(); ++i) 
    {
        if (location_set[i].length() > mx)
        {
            mx = location_set[i].length();
            j = i;
        }
    }
    return (&f.locations[location_set[j]]);
}


// https://stackoverflow.com/questions/12489/how-do-you-get-a-directory-listing-in-c

void cgi__();



// uri code not error in header or somethibg
void    s(location *l, std::string file, uri &i)
{

    if (l->cgi_param.size())
        cgi__();
    else
    {
        struct stat buf;
        if (!stat(file.c_str(), &buf))
        {
            // error 404
        }
        if (S_ISREG(buf.st_mode))
        {
            // read it and sendit
        }
        else if (S_ISDIR(buf.st_mode))
        {
            // handle directory

        }
    }
}

const char *get_content_type(const char * path)
{
    const char *last_dot = strrchr(path, '.');
    if (last_dot)
    {
        if (strcmp(last_dot, ".css") == 0) return "text/css";
        if (strcmp(last_dot, ".csv") == 0) return "text/csv";
        if (strcmp(last_dot, ".gif") == 0) return "image/gif";
        if (strcmp(last_dot, ".htm") == 0) return "text/html";
        if (strcmp(last_dot, ".html") == 0) return "text/html";
        if (strcmp(last_dot, ".ico") == 0) return "image/x-icon";
        if (strcmp(last_dot, ".jpeg") == 0) return "image/jpeg";
        if (strcmp(last_dot, ".jpg") == 0) return "image/jpeg";
        if (strcmp(last_dot, ".js") == 0) return "application/javascript";
        if (strcmp(last_dot, ".json") == 0) return "application/json";
        if (strcmp(last_dot, ".png") == 0) return "image/png";
        if (strcmp(last_dot, ".pdf") == 0) return "application/pdf";
        if (strcmp(last_dot, ".svg") == 0) return "image/svg+xml";
        if (strcmp(last_dot, ".txt") == 0) return "text/plain";
    }
    return "application/octet-stream";
}

void generate_header()
{

}

void    save_regular_file(location *l, std::string file, uri &i, std::string a, response &res)
{
    std::fstream    ifs(file);
    std::string     to_send; //https://stackoverflow.com/questions/2602013/read-whole-ascii-file-into-c-stdstring
    

    // https://cplusplus.com/reference/istream/istream/tellg/
    //i send base on length of file and not of length of the string because we can have '\0' in the middle 
    //this idea is based on reference in slack;
    ifs.seekg (0, ifs.end);
    int length = ifs.tellg();
    ifs.seekg (0, ifs.beg);

    
    std::stringstream strStream;
    strStream << ifs.rdbuf(); //read the file
    res.body = strStream.str(); //str holds the content of the file
    char buf[30];
    res.content_type = sprintf(buf, "%d", length);
}


// MSG_DONTWAIT    in send



namespace timeout
{
    int child_done = 0;
    int timeout = 0;
    void child_handler(int sig)
    {
        child_done = 1;
    }

    void alarm_handler(int sig)
    {
        timeout = 1;
    }

    void    siglisten()
    {
        signal(SIGALRM, alarm_handler);
        signal(SIGCHLD, child_handler);
    }
// https://stackoverflow.com/questions/40223722/how-to-stop-a-child-process-if-not-completed-after-a-timeout
    void    settimeout(int t, pid_t pid)
    {
        alarm(t);  // install an alarm to be fired after TIME_LIMIT
        pause();
        if (timeout)
        {
            int result = waitpid(pid, NULL, WNOHANG);
            if (result == 0)
                kill(pid, 9); // child still running, so kill it
            // else he terminate normally
        }
        else
            wait(NULL); //printf("child finished normally\n");
    }
};

void    cgi__()
{
    
}