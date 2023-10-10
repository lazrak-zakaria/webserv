#include"../hpp/headers.hpp"

int main(int ac, char **av, char **env)
{
	time_t d = time(NULL);
	std::string s = ctime(&d);
	s.pop_back();
	std::cout << "|" << s << "|" << "\n";
	return 0;
}