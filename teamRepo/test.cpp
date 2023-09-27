#include"hpp/headers.hpp"

int main()
{
	struct stat sb;
	if (stat("./test", &sb) == 0)
	{
		if (S_ISDIR(sb.st_mode))
		{
			
		}
	}
	return 0;
}