#include "server.hpp"

int main(int ac, char **av)
{
    if (ac != 2) return 0;

    server  server1;
    server1.run();
}
