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

typedef struct l
{
	ifstream ifs;
	std::string body;
	std::string	pth;

	bool	i_am_sending;
} l;

void	get(l res)
{
	if (!res.i_am_sending)
	{
		
	}
	else
	{

	}
}

int main()
{
	return 0;
}



/*

from get give something to the client to the server then send
the response class
  yes better handle get in response
	if cgi or directory handl it in one part;

first send header;

*/