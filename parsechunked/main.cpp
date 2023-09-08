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
#include <fcntl.h>
#include <string>
#define S 1

int jj = 0;

void	fun(std::string &pu, int &si, int fd1, size_t pos)
{
        if (pos != string::npos && !si)
        {
            // cout << pu << "\n";
            if (pu[0] == '0')
            {
                pu = "";
				return ;
            }
            pu.erase(0, pos + 4);
          //  std::cout << pu << "--------------\n";

            si = 1;
            //continue;
        }
        else if (si && pos != string::npos)
        {
            std::string g = pu.substr(0, pos);
            write(fd1, g.c_str(), g.size());
            //std::cout << g << "\n";
            pu = pu.substr(pos + 4);
            si = 0;
        }
		if (pos == string::npos && pu.length() > 4 && si)
		{
			std::string g = pu.substr(0, pu.length() - 4);
            write(fd1, g.c_str(), g.size());
            //std::cout << g << "\n";
           	pu = pu.substr(pu.length() - 4);
		}
		if(pos != string::npos)
			if (pu.find("\\r\\n") != string::npos)
				fun(pu, si, fd1, pu.find("\\r\\n"));
		
}


int main()
{
    int fd = open("./file", O_RDWR);
    int fd1 = open("./filewri", O_RDWR);
    char buf[5000];
    int r = read(fd, buf, S);
    int si = 0;
    int i = 0;
    char a[10] = {};
    std::string pu;
    while (r > 0)
    {
        buf[r] = 0;
		std::cout << buf<<"\n";
        pu += buf ;
	    size_t pos = pu.find("\\r\\n");
		// if(pos != string::npos)
			fun(pu, si, fd1, pos);
        // if (jj == 16)
        //     while(1);
        r = read(fd, buf, S);
    }

}
