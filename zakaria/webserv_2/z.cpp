#include <cstdio>
#include <cstdlib>
#include <filesystem>
#include <iostream>
namespace fs = std::filesystem;
#include <string>
#include <time.h>
#include <sys/time.h>

void	generateRandomName(std::string &name)
{
	struct timeval	tm;
	srand(time(NULL));
	for (u_int8_t i = 0; i < 15; ++i)
	{
		gettimeofday(&tm, NULL);
		size_t k = rand() + tm.tv_usec;
		switch (k % 3)
		{
			case 0:
				name.push_back((rand() + tm.tv_usec) % 26 + 'a');
			case 1:
				name.push_back((rand() + tm.tv_usec ) % 10+ '0');
			case 2:
				name.push_back((rand() + tm.tv_sec) % 26 + 'A');
		}
	}
}

int main()
{
	std::string name;

}