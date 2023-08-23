#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/types.h>
#include <netdb.h>
#include <errno.h>


int  main()
{
    int i = open("file1", O_RDWR);
    int j = open("file2", O_RDWR);

    fd_set  r, w;
    FD_ZERO(&r);
    FD_ZERO(&w);
    FD_SET(i, &r);
    FD_SET(j, &r);

            char buf[7000];
        struct timeval t;
        t.tv_sec = 1;
        t.tv_usec = 0;
    while (1)
    {
        w = r;
        printf("FIIIIIRST\n");
        //printf("%d", select(5, 0, &r,0,&t));
        if (FD_ISSET(i, &r))
        {
            bzero(buf, 7000);
            read(i, buf, 6999);
            printf("%s HERE\n", buf);
            return ;
        }
        else if (FD_ISSET(j, &r))
        {
            bzero(buf, 70);
            read(j, buf, 69);
            printf("YOYOYOYO\n");
        }
    }
}

