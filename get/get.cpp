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
#include <fstream>
#define S 5 
using namespace std;
#include <dirent.h>
#include <map>

typedef struct l
{
	ifstream ifs;
	std::string body;
	std::string	path;

	std::map<std::string, std::string> cgi;

	bool	i_am_sending;
	bool	done;
} l;

// the part that i check if reg ... it will be shared
void	build_header_send()
{

}

/* forget about socket just code like checking a pth now*/
int autox = 1;
int indx = 1;
void	get(std::string path)
{
	struct stat sb;
	if (stat(path.c_str(), &sb) == 0)
	{
		if (S_ISREG(sb.st_mode))
		{
			ifstream ifs;
			ifs.open(path.c_str());
			if (!ifs.is_open())
			{
				std::cout << "permission denied\n";
				exit(0);
			}
			char a[10];
			while (1)
			{
				ifs.read (a,9);
				if(!ifs.gcount()) break;
				a[ifs.gcount()] = '\0';
				std::cout << a;
			}
			// cout << "\n";
		}
		else if (S_ISDIR(sb.st_mode))
		{
			if (indx)
			{
				if (path[path.size() - 1] != '/')
					path+='/';
				path+= "index";
				ifstream ifs;
				ifs.open(path.c_str());
				if (!ifs.is_open())
				{
					std::cout << "permission denied\n";
					exit(0);
				}
				char a[10];
				while (1)
				{
					ifs.read (a,9);
					if(!ifs.gcount()) break;
					a[ifs.gcount()] = '\0';
					std::cout << a;
				}
			}
			else if (autox)
			{
				  	DIR *d;
  					struct dirent *dir;
					d = opendir(path.c_str());
					if (d)
					{
    					while ((dir = readdir(d)) != NULL) {
      						printf("%s\n", dir->d_name);
    					}
    					closedir(d);
  					}				
			}
		}
	}
}

int main()
{
	get("./dir");
	return 0;
}



/*

from get give something to the client to the server then send
the response class
  yes better handle get in response
	if cgi or directory handl it in one part;

first send header;

*/