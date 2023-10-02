#include <stdio.h>
 #include <time.h>

int main( int argc, char * argv[] ) {

    time_t timestamp = time ( NULL );
    printf ( "Date and local time: %s\n", ctime ( & timestamp ) );
            
    return 0;
}