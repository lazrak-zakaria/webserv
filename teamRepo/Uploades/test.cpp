#include <string>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <unistd.h>
#include <iostream>

#include <vector>

#define N 5
const int size = 10;

struct c
{
	int	id;
	int pid;
	size_t	date_first;
};

int	main()
{
	std::vector<struct c> vec(size);
	for (int i = 0; i < size; ++i)
		vec[i].id = i + 1;

	for (int i = 0; i < size; ++i)
	{
		struct timeval tm;
		gettimeofday(&tm, NULL);
		vec[i].date_first = tm.tv_sec;
		vec[i].pid = fork();
		if (!vec[i].pid)
		{
			sleep(500);
			exit(0);
		}
	}
	int f = 0;
	while (1)
	{
		f = 0;
		for(int i = 0; i < size; ++i)
		{
			if (vec[i].pid == -1)
			{
				f++;
				continue;
			}

			int x = waitpid(vec[i].pid, NULL, WNOHANG);
			if (x == vec[i].pid)
			{
				vec[i].pid = -1;
				std::cout << vec[i].id << " finished\n";
			}
			else if (x == 0)
			{
				struct timeval tm;
				gettimeofday(&tm, NULL);
				if (tm.tv_sec - vec[i].date_first > 1)
				{
					std::cout << vec[i].id << " timeout\n";
					if (kill(vec[i].pid, SIGKILL) == 0)
					{
						std::cout << "killsuccess " << vec[i].pid << '\n';
					}
					// kill(vec[i].pid, SIGINT);
					// kill(vec[i].pid, SIGSEGV);
					// // sleep(1);
					// if (waitpid(vec[i].pid, NULL, 0) != vec[i].pid)
					// 	std::cout << "error\n";
					vec[i].pid = -1;
				}
			}
		}
		if (f == size)
			break;
	}
	struct timeval tm;
	gettimeofday(&tm, NULL);
	std::cout << tm.tv_sec << "\n";
	while(1);
	return 0;
}

