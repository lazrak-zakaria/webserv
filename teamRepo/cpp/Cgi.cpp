#include "../hpp/Client.hpp"



void	Client::Cgi::executeCgi()
{


	struct timeval tmv;
	gettimeofday(&tmv, NULL);
	timeStart =  tmv.tv_sec;

	/*open outputfile here*/
	
	me->generateRandomName(outputFileCGi);
	outputFileCGi = std::string("./../tmp/").append(outputFileCGi);
	me->_response.inputFile.open(outputFileCGi);
	if (me->_response.inputFile.is_open() == false)
	{
		me->_codeStatus = 500;
		return ;
	}
	processPid = fork();
	if (processPid == 0)
	{
		/*Setup env variables*/
		char* env[10];
		env[0] = NULL;



		std::string	&programName 	= me->_configData->allLocations[me->_locationKey].cgi[cgiKeyProgram];
		std::string	&scriptToexec	= me->_finalPath;
		
		if (inputFileCGi.empty() == false)
		{
			if (freopen(inputFileCGi.c_str(), "r", stdin) == NULL)
				exit(1);
		}
		char *argv[3];
		
		argv[0] = strdup(programName.c_str());
		argv[1] = strdup(scriptToexec.c_str());
		argv[2] = NULL;

		execve(argv[0], argv, env);
	}
	else
	{
		int ret = waitpid(processPid, 0, WNOHANG);
		if (ret == processPid)
			me->_flags.canReadInputFile = true;
		else if (ret == 0)
			me->_flags.isCgiRunning = true;
		else
			me->_codeStatus = 500;
	}
}
