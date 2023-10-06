#include <iostream>
#include "./teamRepo/hpp/headers.hpp"


std::map<std::string, std::string> cgiHeadersMap;

int codeStatus;


    std::string statusLine;
void f()
{
    if (cgiHeadersMap.count("status"))
	{
        size_t i = 0;
        u_int8_t j = 0;
        std::string tmpCodeStatus;
        std::string errorDescription;

        while (cgiHeadersMap["status"][i] == ' ') ++i;
        while (cgiHeadersMap["status"][i] && cgiHeadersMap["status"][i] != ' ')
        {
            if (!isdigit(cgiHeadersMap["status"][i]) || j > 2)
            {

                codeStatus = 502;
                return ;
            }
            else
                tmpCodeStatus.push_back(cgiHeadersMap["status"][i]);
            ++i;
            ++j;
        }
        if (tmpCodeStatus > "599" || j != 3 || cgiHeadersMap["status"][i] != ' ')
        {
            codeStatus = 502;
            return ;
        }

        ++i;

        statusLine = std::string("HTTP/1.1 ").append(tmpCodeStatus);
        statusLine.push_back(' ');
        statusLine.append(cgiHeadersMap["status"].substr(i));
        statusLine.append("\r\n");
    }
    else
        statusLine = "HTTP/1.1 200 OK\r\n";


}


int main()
{
   cgiHeadersMap["status"] = "  502";
   f();
   std::cout << statusLine << "|" << codeStatus << "\n";
    return 0;
}
