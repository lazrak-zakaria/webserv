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

#define S 3

int jj = 0;
int main()
{
    int fd = open("./file", O_RDWR);
    int fd1 = open("./filewri", O_RDWR);
    char buf[5000];
    int r = read(fd, buf, S);
    size_t si = 0;
    int i = 0;
    char a[10] = {};
    std::string pu;
    while (r > 0)
    {
        buf[r] = 0;
        pu += buf ;
        size_t pos = pu.find("\\r\\n");
        if (pos != string::npos && !si)
        {
            // cout << pu << "\n";
            if (pu[0] == '0')
            {
                pu = "";
                break;
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
        ++jj;
        if (jj == 16)
            while(1);
        r = read(fd, buf, S);
    }

}


