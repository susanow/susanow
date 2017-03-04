
#include "vty.h"

int main()
{
    int port = rand()%8  + 9990 ;
    printf("server: %d/tcp \n", port);
    vty vty(port);
    vty.dispatch();
}

