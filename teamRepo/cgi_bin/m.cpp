#include <iostream>
#include <unistd.h>
int main(int c, char **av, char **env)
{
//     for (int i = 0; env[i]; ++i)
//         std::cerr << env[i] << "\n";

//     std::cout << "content-type: text/html\n\n";

//     std::cout << "<!DOCTYPE html>\
// <html lang=\"en\">\
// <head>\
//     <meta charset=\"UTF-8\">\
//     <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\
//     <title>CGI</title>\
// </head>\
// <body>\
//     <h1>CGI</h1>\
// </body>\
// </html>";

    char *a[8000] = {"/usr/bin/php-cgi", "t.php", NULL};
    execve(a[0], a, env);
    std::cerr << "77777777777777777777777777777777777777\n";
    return 0;
}