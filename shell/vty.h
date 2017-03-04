
#pragma once

#include <termios.h>
#include <fcntl.h>
#include <time.h>
#include <netinet/tcp.h>
#include <slankdev/exception.h>
#include <slankdev/socketfd.h>
#include <slankdev/util.h>

#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <poll.h>
#include <fcntl.h>
#include <string>

#include <slankdev/exception.h>
#include <slankdev/socketfd.h>
#include <slankdev/util.h>

#include "telnet.h"
#include "asciicode.h"
#include "shell.h"

#include <vector>



class vty {
    std::vector<struct pollfd> fds;
    std::vector<shell> shells;
public:
    vty(int port)
    {
        slankdev::socketfd sock;
        sock.noclose_in_destruct = true;
        sock.socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        srand(time(NULL));
        struct sockaddr_in addr;
        addr.sin_family = AF_INET;
        addr.sin_port   = htons(port);
        addr.sin_addr.s_addr = INADDR_ANY;
        sock.bind((sockaddr*)&addr, sizeof(addr));
        sock.listen(5);

        struct pollfd pfd;
        pfd.fd = sock.get_fd();
        pfd.events = POLLIN;
        fds.push_back(pfd);

        setbuf(stdout, NULL);
        setbuf(stdin, NULL);
    }
    void dispatch()
    {
        while (1) {
            if (poll(fds.data(), fds.size(), 1000)) {
                if (fds[0].revents & POLLIN) {
                    /*
                     * Server Accept Process
                     */
                    struct sockaddr_in client;
                    socklen_t client_len = sizeof(client);
                    int fd = accept(fds[0].fd, (sockaddr*)&client, &client_len);
                    setbuf(fdopen(fd, "wr"), NULL);
                    slankdev::socketfd client_sock(fd);
                    uint32_t on = 1;
                    client_sock.setsockopt(IPPROTO_TCP, TCP_NODELAY,
                            (char*)&on, sizeof(on));
                    int flags = client_sock.fcntl(F_GETFL);
                    client_sock.fcntl(F_SETFL, (flags | O_NONBLOCK));
                    client_sock.noclose_in_destruct = true;

                    struct pollfd client_pfd;
                    client_pfd.fd = fd;
                    client_pfd.events = POLLIN | POLLERR;
                    fds.push_back(client_pfd);

                    shells.resize(shells.size()+1);
                    shells[shells.size()-1].fd = fd;
                    shells[shells.size()-1].dispatch();
                    printf("Connected new client. now, nb_client=%zd nb_shells=%zd\n",
                            fds.size()-1, shells.size());
                }

                /*
                 * Client Read Process
                 */
                for (size_t i=1; i<fds.size(); i++) {
                    if (fds[i].revents & POLLIN) {
                        int res = shells[i-1].process();
                        if (shells[i-1].closed || res<=0) {
                            close(fds[i].fd);
                            fds.erase(fds.begin()+i);
                            shells.erase(shells.begin() + i);
                            printf("Disconnect client. nb_client=%zd nb_shells=%zd\n", fds.size()-1, shells.size());
                            continue;
                        }
                    }
                }
            }
        }
    }


};




int func()
{
}


