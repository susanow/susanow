
#include "shell.h"
#include "socket.h"

int main()
{
    int fd = get_client_sock();
    shell shell(fd, "Susanw> ");
    shell.dispatch();
}

