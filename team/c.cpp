#include <string>
#include <iostream>


int  main()
{
	std::string a = "1365444545477777779779797979666666666666666666666666666666999999999997";
	std::string b = "";
	a.push_back('1');
	a.clear();
	std::string().swap(a);
	std::cout << a << "|"<< a.capacity() << "\n";
	return 0;
}