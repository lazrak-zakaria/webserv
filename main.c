// #include <stdio.h>
// #include <stdlib.h>
// #include <sys/socket.h>
// #include <sys/select.h>
// #include <string.h>
// #include <unistd.h>
// #include <netinet/in.h>
// #include <arpa/inet.h>
// #include <sys/stat.h>
// #include <fcntl.h>
// #include <sys/types.h>
// #include <netdb.h>
// #include <errno.h>


// int  main()
// {
//     int i = open("file1", O_RDWR);
//     int j = open("file2", O_RDWR);

//     fd_set  r, w;
//     FD_ZERO(&r);
//     FD_ZERO(&w);
//     FD_SET(i, &r);
//     FD_SET(j, &r);

//             char buf[7000];
//         struct timeval t;
//         t.tv_sec = 1;
//         t.tv_usec = 0;
//     while (1)
//     {
//         w = r;
//         printf("FIIIIIRST\n");
//         //printf("%d", select(5, 0, &r,0,&t));
//         if (FD_ISSET(i, &r))
//         {
//             bzero(buf, 7000);
//             read(i, buf, 6999);
//             printf("%s HERE\n", buf);
//             return ;
//         }
//         else if (FD_ISSET(j, &r))
//         {
//             bzero(buf, 70);
//             read(j, buf, 69);
//             printf("YOYOYOYO\n");
//         }
//     }
// }


#include <bits/stdc++.h>
using namespace std;

void tokenize(string s, string del = " ")
{
	    int start = 0, end = 0;
    while (end != std::string::npos)
    {
        end = s.find(del, start);
        if (end != std::string::npos)
        {
            std::string d = s.substr(start, end - start);
           // if (!d.empty())
             //   vec.push_back(d);
             if (!d.empty())
            cout << d << "\n";
            start = end + del.size();
        }
    }
}
int main(int argc, char const* argv[])
{

    char a[] = "zakarialazk";
    a[7] = 0;
    string s = a;
    std::cout << s.capacity() << "\n";
	return 0;
}
