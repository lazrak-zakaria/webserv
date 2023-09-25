#include <string>
#include <iostream>
#include <sstream>

std::string answer;

bool	expectSizeRead = true;
size_t	expectedBytesToRead;
size_t	readAmountSoFar;
bool	crlfRequired;

void	parseCh(std::string &req)
{

	if (expectSizeRead)
	{
		if (size_t pos = req.find("\r\n") && pos != std::string::npos)
		{
			std::string hexValue(req, 0, pos);
			
			std::stringstream ss;
			ss  << hexValue ; 
			ss >> std::hex >> expectedBytesToRead ;
			
			if (expectedBytesToRead == 0)
				return ;

			req.erase(0, pos + 2);
			readAmountSoFar = 0;
			expectSizeRead = false;
		}
	}
	else
	{
		size_t reqSize = req.size();
		std::string	contentToStore;


		// /**/
		// for (size_t i = 0 ; readAmountSoFar < expectedBytesToRead && i < reqSize; ++readAmountSoFar , ++i)
		// 	toWriteToFile.push_back(req[i]);
		// /**/

		if (!crlfRequired)
		{
			if (reqSize >= (expectedBytesToRead - readAmountSoFar))
			{
				contentToStore.append(req, 0, (expectedBytesToRead - readAmountSoFar));

				std::cout << "*"<< req << "*" << (expectedBytesToRead - readAmountSoFar) << "*" << contentToStore <<"*XX\n";
				req.erase(0, (expectedBytesToRead - readAmountSoFar));
				readAmountSoFar = expectedBytesToRead = 0;
				crlfRequired = true;
			}
			else
			{
				contentToStore.append(req);
				req.erase(0, reqSize);
				readAmountSoFar += reqSize;

			}
			answer.append(contentToStore);
		}

		if (crlfRequired && req.size() >= 2)
		{
			if (req.compare(0, 2, "\r\n") == 0)
			{
				
				crlfRequired = false;
				req.erase(0, 2);
			}
			else
				std::cout << "Error\n";
			
			expectSizeRead = true;
		}
	}
	
	if ((expectSizeRead && req.find("\r\n")) || (expectSizeRead == 0 && req.empty() == 0))
		parseCh(req);
	
}




int  main()
{
	std::string a = "5\r\nabcde\r\n3\r\nabc";
	// std::string a = "\r\ndat\r\n";
	// std::string h = "abcde";
	parseCh(a);
	// answer.append(h, 0,3);
	std::cout << "|" << answer << "|\n";
	return 0;
}