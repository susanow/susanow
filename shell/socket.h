
#pragma once

#include <termios.h>
#include <fcntl.h>
#include <time.h>
#include <netinet/tcp.h>
#include <slankdev/exception.h>
#include <slankdev/socketfd.h>
#include <slankdev/util.h>



int get_client_sock()
{
    slankdev::socketfd sock;
    sock.socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    srand(time(NULL));
    int port = rand()%8  + 9990 ;

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port   = htons(port);
    addr.sin_addr.s_addr = INADDR_ANY;
    sock.bind((sockaddr*)&addr, sizeof(addr));

    printf("server: %d/tcp \n", port);
    sock.listen(5);
    struct sockaddr_in client;
    socklen_t client_len = sizeof(client);
    int fd = sock.accept((sockaddr*)&client, &client_len);

    slankdev::socketfd client_sock(fd);
    uint32_t on = 1;
    client_sock.setsockopt(IPPROTO_TCP, TCP_NODELAY, (char*)&on, sizeof(on));
    int flags = client_sock.fcntl(F_GETFL);
    client_sock.fcntl(F_SETFL, (flags | O_NONBLOCK));
    client_sock.noclose_in_destruct = true;

    struct pollfd fds[2];
    memset(fds, 0, sizeof(fds));
    fds[0].fd = fd;
    fds[0].events = POLLIN | POLLERR;
    setbuf(stdout, NULL);
    setbuf(stdin, NULL);
    setbuf(fdopen(fd, "wr"), NULL);

    return fd;
}
