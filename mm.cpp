#include <stdio.h>
#include "wb/header.hpp"
#include <libgen.h>
int main ()
{
	std::string path = "./wb/cpp/cgi.cpp";

	char *a = dirname(strdup(path.c_str()));
	std::cout << a << "\n";
  return 0;
}