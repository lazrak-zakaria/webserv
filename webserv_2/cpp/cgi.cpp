#include "../client.hpp"


/**********				cgi				**********/
void	client::cgi::cgi_clear()
{
	env_variables.clear();
	output_file_name.clear();
}

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
        else if (child_done)
        {
			wait(NULL); //printf("child finished normally\n");
			return 1;
		}
		return 1;
	}
};

// http://isp.vsi.ru/library/Perl/CGI/ch6.htm
void	client::cgi::execute_cgi(void)
{
	env_variables["SERVER_NAME: "] = me->config_data->host;
	env_variables["SERVER_PROTOCOL: "] = "HTTP/1.1";
	// env_variables["SERVER_PORT"] = me->config_data->port;
	env_variables["REQUEST_METHOD: "] = me->request.method;
	// env_variables["PATH_INFO"];
	if (me->request.request_headers.count("content-length"))
		env_variables["CONTENT_LENGTH: "] = me->request.request_headers["content-length"];
	if (me->request.request_headers.count("content-type"))
	{
		env_variables["CONTENT_TYPE: "] = me->request.request_headers["content-type"];
	}
	if (!me->request.query_string.empty())
		env_variables["QUERY_STRING: "] =  me->request.query_string;



	std::string toexec__[2];
	/*
		make sure final path not end with /

	*/
	size_t	pos = me->final_path.find_last_of('/') + 1;
	std::string	scriptname = me->final_path.substr(pos);
	toexec__[1] = scriptname;
	pos = scriptname.find_last_of('.');
	if (pos == std::string::npos)
	{
		me->code_status = 404;
		return ;
	}
	toexec__[0] = scriptname.substr(pos+1).c_str();

	char a[] = "./Temporary/fileXXXXXX";
	int fd = mkstemp64(a);
	if (fd == -1)
	{
		me->code_status = 500;
		return ;
	}
	output_file_name = a;
	pid_t child_pid = fork();
	if (!child_pid)
	{
		char **env= new char*[env_variables.size() + 1];
		size_t	i = 0;
		std::map<std::string , std::string>::iterator it;
		for (it = env_variables.begin(); it != env_variables.end(); ++it)
		{
			std::string tmp = it->first + it->second;
			env[i++] = strdup(tmp.c_str());
		}
		env[i] = NULL;
		freopen64(me->request.file_name.c_str(), "r", stdin);//check
		freopen64(a, "w", stdout);//check
		std::string  correct_directory = me->final_path.substr(me->final_path.find_last_of('/'));
		chdir(correct_directory.c_str());//check
		char *texec[3] = {strdup(toexec__[0].c_str()), strdup(toexec__[1].c_str()), NULL};
		execve(texec[0], texec, env);
		exit(0);
	}
	timeout::settimeout(2, child_pid);

}
