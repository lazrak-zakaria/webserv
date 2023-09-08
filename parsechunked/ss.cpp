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
