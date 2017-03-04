
#include "vty.h"

int main()
{
    int port = rand()%8  + 9990 ;
    vty vty(port);
    vty.dispatch();
}

