#include <iostream>

int main()
{
    std::cout << "content-tydpe: text/html\n\n";
    std::cout << "<!DOCTYPE html>\
<html lang=\"en\">\
<head>\
    <meta charset=\"UTF-8\">\
    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\
    <title>CGI</title>\
</head>\
<body>\
    <h1>CGI</h1>\
</body>\
</html>";
    return 0;
}