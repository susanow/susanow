
#pragma once
#include <slankdev/vty.h>

using slankdev::vty;


class Cmd_show : public slankdev::vty::cmd_node {
    struct author : public cmd_node {
        author() : cmd_node("author") {}
        void function(vty::shell* sh)
        {
            sh->Printf("Hiroki SHIROKURA.\r\n");
            sh->Printf(" Twitter : @slankdev\r\n");
            sh->Printf(" Github  : slankdev\r\n");
            sh->Printf(" Facebook: hiroki.shirokura\r\n");
            sh->Printf(" E-mail  : slank.dev@gmail.com\r\n");
        }
    };
    struct version : public cmd_node {
        version() : cmd_node("version") {}
        void function(vty::shell* sh)
        {
            sh->Printf("Susanow 0.0.0\r\n");
            sh->Printf("Copyright 2017-2020 Hiroki SHIROKURA.\r\n");
        }
    };
    struct thread_info : public cmd_node {
        thread_info() : cmd_node("thread-info") {}
        void function(vty::shell* sh)
        {
            sh->Printf("show thread-info\r\n");
        }
    };
    struct port : public cmd_node {
        port() : cmd_node("port") {}
        void function(vty::shell* sh) { sh->Printf("show port \r\n"); }
    };
    struct cpu : public cmd_node {
        cpu() : cmd_node("cpu") {}
        void function(vty::shell* sh)
        {
            sh->Printf("show cpu \r\n");
        }
    };
public:
    Cmd_show() : cmd_node("show")
    {
        commands.push_back(new author);
        commands.push_back(new version);
        commands.push_back(new thread_info);
        commands.push_back(new port);
        commands.push_back(new cpu);
    }
    void function(vty::shell* sh) { sh->Printf("show\r\n"); }
};
struct Cmd_quit : public slankdev::vty::cmd_node {
    Cmd_quit() : cmd_node("quit") {}
    void function(vty::shell* sh) { sh->close(); }
};
struct halt : public slankdev::vty::cmd_node {
    halt() : cmd_node("halt") {}
    void function(vty::shell* sh)
    {
        sh->root_vty->halt();
    }
};
struct clear : public slankdev::vty::cmd_node {
    clear() : cmd_node("clear") {}
    void function(vty::shell* sh)
    {
        sh->Printf("\033[2J\r\n");
    }
};






#include <slankdev/dpdk_header.h>
void lscpu()
{
    printf("Architecture        : \n");
    printf("CPU op-mode(s)      : \n");

    uint16_t before = 0x1234;
    uint16_t after  = rte_le_to_cpu_16(before);
    printf("Byte Order          : %s \n",before==after?"Little Endian":"Big Endian");

    printf("CPU(s):             : \n");
    printf("On-line CPU(s) list : \n");
    printf("Thread(s) per core  : \n");
    printf("Core(s) per socket  : \n");
    printf("Socket(s)           : \n");
    printf("NUMA node(s)        : \n");
    printf("Vendor ID           : \n");
    printf("CPU family          : \n");
    printf("Model               : \n");
    printf("Model name          : \n");
    printf("Stepping            : \n");
    printf("CPU MHz             : \n");
    printf("CPU max MHz         : \n");
    printf("CPU min MHz         : \n");
    printf("BogoMIPS            : \n");
    printf("Virtualization      : \n");
    printf("L1d cache           : \n");
    printf("L1i cache           : \n");
    printf("L2 cache            : \n");
    printf("L3 cache            : \n");
    printf("NUMA node0 CPU(s)   : \n");

    printf("Flags               : ");
    // for (size_t i=0; i<RTE_CPUFLAG_NUMFLAGS; i++) {
    //     if (sys->cpuflags[i])
    //         printf("%s ", rte_cpu_get_flag_name(rte_cpu_flag_t(i)));
    // }
    printf("\n");
}

