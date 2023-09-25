// C++ program to create a temporary file
// read and write to a temporary file.
#include <cstdio>
#include <cstdlib>
#include <iostream>

using namespace std;

// Driver code
int main()
{
	// Creating a file pointer which
	// points to the temporary file
	// created by tmpfile() method
	FILE* fp = tmpfile();

	// Content to be written in temporary file
	char write[] = "Welcome to Geeks For Geeks";

	// If file pointer is NULL there is
	// error in creating the file
	if (fp == NULL)
	{
		perror("Error creating temporary file");
		exit(1);
	}

	// Writing in temporary file the content
	fputs(write, fp);
	rewind(fp);

	// Reading content from temporary file
	// and displaying it
	char read[100];
	fgets(read, sizeof(read), fp);
	cout << read;

	// Closing the file. Temporary file will
	// also be deleted here
    while(1);
	fclose(fp);

	return 0;
}
