#include <bits/stdc++.h>
using namespace std;
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>

int main()
{
    int fd[2];
    string a = "zazazazazaza2000";
                //  "zazazazaza2000"
    char aa[500];
    pipe(fd);
    pid_t t = fork();
    if (!t)
    {
        dup2(fd[0], STDIN_FILENO);
        dup2(fd[1], STDOUT_FILENO);
        write(fd[1], a.c_str(), a.length());
        // close(fd[1]);
        // close(fd[0]);
        string k = "./test";
        char *b[] = {"./test", NULL};
        execve(k.c_str(), b, 0);
        exit(0);
    }
    wait(0);
    close(fd[1]);
    int f = read(fd[0], aa, 494);
    aa[f] = 0;
    close(fd[0]);
    std::cout << f << "  " <<  aa;

}

