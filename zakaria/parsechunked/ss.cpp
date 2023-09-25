//  #include <bits/stdc++.h>
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
#include <iostream>
#define S 5 
using namespace std;

int jj = 0;

void	fun(std::string &pu, int &si, int &fd1, size_t pos, std::string bound)
{
	if (!si && pu.length() + 4 >= pos)
	{
		if (pu.find(bound) != string::npos)
			pu.erase(0, pu.find(bound) + bound.length() + 4);
		if (pu.find("\\r\\n\\r\\n") != string::npos)
		{
			if (pu.find("Content-Disposition:")!= string::npos )
			{
				if (pu.find("filename=")!= string::npos)
				{
					// fd1 = open("./secwri", O_RDWR);
					char a[] = "zXXXXXX";
					fd1 = mkstemp(a);
					std::cout << "------------->	"<< a << "\n";
					// std::cout << "h--------------\n";
				}
			}
			if (pu.find( "Content-Type:")!= string::npos)
			{
				std::cout << pu << "\n";
				pu.erase(0, pu.find("\\r\\n\\r\\n") + 8);
				si = 1;
			}
		}
	}
	if (si)
	{
		if (pu.find("\\r\\n") != string::npos)
		{
			string g = pu.substr(0,(pu.find("\\r\\n")));
			write(fd1, g.c_str(), g.length());
			pu = pu.substr(pu.find("\\r\\n") + 4);
			si = 0;
		}
		else if (pu.length() > pos && fd1)
		{
			string g = pu.substr(0,pu.length() - pos);
			write(fd1, g.c_str(), g.length());
			pu = pu.substr(pu.length() - pos);
		}
	}
	
}


int main()
{
    int fd = open("./sec", O_RDWR);
    // int fd1 = open("./filewri", O_RDWR);
	int fd1 = 0;
    char buf[5000];
    int r = read(fd, buf, S);
    int si = 0;
    int i = 0;
    char a[10] = {};
    std::string pu;
	size_t pos = 14;
	std::string bound = "--boundary123";
    while (r > 0)
    {
        buf[r] = 0;
		//// std::cout << buf<<"\n";
        pu += buf ;
		// if(pos != string::npos)
		fun(pu, si, fd1, pos, bound);
        // if (jj == 16)
        //     while(1);
        r = read(fd, buf, S);
    }

}
