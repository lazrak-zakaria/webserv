#include"hpp/headers.hpp"

int main()
{
	std::string a = "aabb";
	if (a.compare(2, 2, "bb") == 0)
		std::cout << "F\n";
}