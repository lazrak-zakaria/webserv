#include "../hpp/Client.hpp"


/*even if kill the process you should wait*/
void Client::Cgi::checkCgiTimeout()
{

	if (waitpid(processPid, 0, WNOHANG) == processPid)
	{
		/*cgi end good && later check exit status*/
		me->_flags.canReadInputFile = true;
	}
	else
	{
		struct timeval tmv;
		gettimeofday(&tmv, NULL);
		size_t timeNow =  tmv.tv_sec;
		if (timeNow - timeStart > 7)
		{
			kill(processPid, SIGKILL);
			waitpid(processPid, 0, WNOHANG);
			me->_codeStatus = 504;
		}
	}
}

void	Client::Cgi::executeCgi()
{



	/*open outputfile here*/
	me->generateRandomName(outputFileCGi);
	outputFileCGi = std::string("./../tmp/TTT").append(outputFileCGi);
	std::ofstream outfile (outputFileCGi);
	if (outfile.is_open() == false)
	{
		me->_codeStatus = 500;
		return ;
	}
	outfile.close();
	processPid = fork();
	if (processPid == 0)
	{
		/*Setup env variables*/
		char* env[10];
		env[0] = NULL;

		me->_response.inputFile.close();

		std::string	&programName 	= me->_configData->allLocations[me->_locationKey].cgi[cgiKeyProgram];
		std::string	&scriptToexec	= me->_finalPath;
		
		if (inputFileCGi.empty() == false) /*for post*/
		{
			if (freopen(inputFileCGi.c_str(), "r", stdin) == NULL)
			{
				std::cout << "CGI INPUT FAIL\n";
				exit(1);
			}
		}
		if (freopen(outputFileCGi.c_str(), "w", stdout) == NULL)
		{
			std::cout << "CGI outPUT FAIL\n";
			exit(1);
		}
		char *argv[3];
		
		argv[0] = strdup(programName.c_str());
		argv[1] = strdup(scriptToexec.c_str());
		argv[2] = NULL;
		exit(5);
		execve(argv[0], argv, env);
	}
	else
	{
		struct timeval tmv;
		gettimeofday(&tmv, NULL);
		timeStart =  tmv.tv_sec;

		int ret = waitpid(processPid, 0, WNOHANG);
		if (ret == processPid)
			me->_flags.canReadInputFile = true;
		else if (ret == 0)
			me->_flags.isCgiRunning = true;
		else
		

		me->_response.inputFile.open(outputFileCGi);
		if (me->_response.inputFile.is_open())
				me->_codeStatus = 500;
	}
}
