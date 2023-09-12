#include ".././cgi.hpp"

namespace timeout
{
    int child_done = 0;
    int timeout = 0;
    void child_handler(int sig)
    {
        child_done = 1;
    }

    void alarm_handler(int sig)
    {
        timeout = 1;
    }

    void    siglisten()
    {
        signal(SIGALRM, alarm_handler);
        signal(SIGCHLD, child_handler);
    }
// https://stackoverflow.com/questions/40223722/how-to-stop-a-child-process-if-not-completed-after-a-timeout
    int   settimeout(int t, pid_t pid)
    {
        alarm(t);  // install an alarm to be fired after TIME_LIMIT
        pause();
        if (timeout)
        {
            int result = waitpid(pid, NULL, WNOHANG);
            if (result == 0)
                kill(pid, 9); // child still running, so kill it
            // else he terminate normally
			return 0;
		}
        else
        {
			wait(NULL); //printf("child finished normally\n");
			return 1;
		}

	}
};

void	cgi::execute_cgi(server &server_conf, request &http_request)
{
	REQUEST_METHOD = "REQUEST_METHOD: " + http_request.uri.method;
	env_variables.push_back(REQUEST_METHOD); // later use only this
	if (!http_request.is_chunked && REQUEST_METHOD == "POST")
	{
		CONTENT_LENGTH = "CONTENT_LENGTH: " + http_request.header_fields["content-length"][0];
		env_variables.push_back(CONTENT_LENGTH);
	}
	if (REQUEST_METHOD == "POST")
	{
		CONTENT_TYPE = "CONTENT_TYPE: ";
		for (size_t i = 0; i < http_request.header_fields["content-type"].size(); ++i)
		{
			CONTENT_TYPE +=  http_request.header_fields["content-type"][i];
			if (i + 1 != http_request.header_fields["content-type"].size())
				CONTENT_TYPE.push_back(' ');
		}
		env_variables.push_back(CONTENT_TYPE);
	}
	else if (REQUEST_METHOD == "GET")
	{
		QUERY_STRING = "QUERY_STRING: " + http_request.uri.query_string;
		env_variables.push_back(QUERY_STRING);
	}

	pid_t pid = fork();
	// -1 later
	if (!pid)
	{
		// find last directory then chdir to it the run  	char *a = dirname(strdup(path.c_str()));
		char **env= new char*[env_variables.size() + 1];
		size_t i = 0;
		for (i = 0; i < env_variables.size(); ++i)
			env[i] = strdup(env_variables[i].c_str());
		env[i] = NULL;
		size_t	pos = http_request.final_path.find_last_of('/');
		char *to_execute[3];
		to_execute[1] = strdup(http_request.final_path.c_str());
		char ext[] = ".py"; // retrieve this;
		to_execute[0] = strdup(server_conf.all_locations[http_request.key_location].cgi[ext].c_str());
		to_execute[2] = NULL;
		//	make the file as input;
		//	make another file as output;
		//	use freopen;
		freopen(http_request.file_name.c_str(), "r", stdin);
		freopen(http_request.cgi_output_file_name.c_str(), "w", stdout);
		execve(to_execute[0], to_execute, env);
		// no need to delete allocated
		exit(1);
	}
	else
	{
		if (timeout::settimeout(2, pid))
		{

		}
		else
		{
			//cgi took long time;
		}
	}

	
}
