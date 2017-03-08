
#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include <unistd.h>
#include <time.h>
#include <poll.h>
#include <fcntl.h>
#include <netinet/tcp.h>

#include <string>
#include <vector>

#include <slankdev/socketfd.h>
#include <slankdev/util.h>

#include "shell.h"
#include "telnet.h"


class vty {
    int server_fd;
    std::vector<shell> shells;
    static uint16_t port_default;
    bool running;

public:
    static void add_keyfunction(KeyFunc* kf)
    {
        shell::keyfuncs.push_back(kf);
        printf("Add Keyfunction \"0x%02x\"\n", kf->code);
    }
    static void add_command(node* cmd)
    {
        shell::commands.push_back(cmd);
        printf("Add Command, \"%s\"\n", cmd->name.c_str());
    }
    static void set_port(uint16_t p)
    {
        port_default = p;
        printf("Set port number %u \n", port_default);
    }

public:
    vty() : running(false)
    {
        slankdev::socketfd server_sock;
        server_sock.noclose_in_destruct = true;
        server_sock.socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        int option = 1;
        server_sock.setsockopt(SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option));
        struct sockaddr_in addr;
        addr.sin_family = AF_INET;
        addr.sin_port   = htons(port_default);
        addr.sin_addr.s_addr = INADDR_ANY;
        server_sock.bind((sockaddr*)&addr, sizeof(addr));
        server_sock.listen(5);
        server_fd = server_sock.get_fd();
    }
    virtual ~vty() {}
    void halt() { running = false; }
    void dispatch()
    {
        running = true;
        while (running) {
            struct Pollfd : public pollfd {
                Pollfd(int ifd, short ievents)
                {
                    fd = ifd;
                    events = ievents;
                }
            };
            std::vector<struct Pollfd> fds;
            fds.push_back(Pollfd(server_fd, POLLIN));
            for (shell& sh : shells) fds.emplace_back(Pollfd(sh.fd, POLLIN));

            if (poll(fds.data(), fds.size(), 1000)) {
                if (fds[0].revents & POLLIN) {
                    /*
                     * Server Accept Process
                     */
                    struct sockaddr_in client;
                    socklen_t client_len = sizeof(client);
                    int fd = accept(fds[0].fd, (sockaddr*)&client, &client_len);

                    slankdev::socketfd client_sock(fd);
                    uint32_t on = 1;
                    client_sock.setsockopt(IPPROTO_TCP, TCP_NODELAY, &on, sizeof(on));
                    int flags = client_sock.fcntl(F_GETFL);
                    client_sock.fcntl(F_SETFL, (flags | O_NONBLOCK));
                    client_sock.noclose_in_destruct = true;
                    vty_will_echo (fd);
                    vty_will_suppress_go_ahead (fd);
                    vty_dont_linemode (fd);
                    vty_do_window_size (fd);

                    shells.resize(shells.size()+1);
                    shells[shells.size()-1].fd = fd;
                    shells[shells.size()-1].dispatch();
                    printf("Connected new client. now, nb_shells=%zd\n", shells.size());
                }

                /*
                 * Client Read Process
                 */
                for (size_t i=1; i<fds.size(); i++) {
                    if (fds[i].revents & POLLIN) {
                        int res = shells[i-1].process();
                        if (shells[i-1].closed || res<=0) {
                            close(fds[i].fd);
                            shells.erase(shells.begin() + i);
                            printf("Disconnect client. nb_shells=%zd\n", shells.size());
                            continue;
                        }
                    }
                }
            }
        }
    }
};
uint16_t vty::port_default = 9999;

