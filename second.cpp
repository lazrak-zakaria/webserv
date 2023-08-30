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
    std::string a = "-------------------------------------------";
    write(2,"blalal\n", 8);
    char rrr[500];
    int o = read(0, rrr, 2);
    rrr[o] = 0;
    string h = rrr ;
    h += "::::::\n";
    write(2, h.c_str(), strlen(h.c_str()));
    // a = a;
    write(1, a.c_str(), a.length());
    // write(2, a.c_str(), a.length());
}