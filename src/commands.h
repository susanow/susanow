

#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <string>

#include <slankdev/util.h>

#include <ssnlib_sys.h>
#include <ssnlib_cmd.h>



class Cmd_quit : public ssnlib::Command {
    System* sys;
public:
    Cmd_quit(const char* n, System* s) : Command(n), sys(s) {}
    void operator()(const std::vector<std::string>& args)
    {
        UNUSED(args);
        sys->halt();
    }
};

class Cmd_clear : public ssnlib::Command {
public:
    Cmd_clear(const char* n) : ssnlib::Command(n) {}
    void operator()(const std::vector<std::string>& args)
    {
        UNUSED(args);
        slankdev::clear_screen();
    }
};


class Cmd_test : public ssnlib::Command {
    System* sys;
    ssnlib::Shell*  shell;
public:
    Cmd_test(const char* n, System* s, ssnlib::Shell* sh)
        : Command(n), sys(s), shell(sh) {}
    void operator()(const std::vector<std::string>& args)
    {
        UNUSED(args);
        for (;;) {
            slankdev::clear_screen();
            shell->exe_cmd("port show");
            usleep(50000);
        }
    }
};
class Cmd_run : public ssnlib::Command {
    System* sys;
    ssnlib::Shell*  shell;
public:
    Cmd_run(const char* n, System* s, ssnlib::Shell* sh)
        : Command(n), sys(s), shell(sh) {}
    void operator()(const std::vector<std::string>& args)
    {
        UNUSED(args);
        shell->exe_cmd("thread launch 2");
        shell->exe_cmd("thread launch 3");
        // shell->exe_cmd("thread launch 4");
    }
};


class Cmd_show : public ssnlib::Command {
    System* sys;
public:
    Cmd_show(const char* n, System* s) : ssnlib::Command(n), sys(s) {}

    void show_cpu()
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
        for (size_t i=0; i<RTE_CPUFLAG_NUMFLAGS; i++) {
            if (sys->cpuflags[i])
                printf("%s ", rte_cpu_get_flag_name(rte_cpu_flag_t(i)));
        }
        printf("\n");
    }
    void show_version()
    {
        printf("Susanoo version 0.0 \n");
    }
    void usage(const std::string& s)
    {
        printf("Usage: %s COMMAND \n", s.c_str());
        printf(" COMMAND := { cpu | version | thread } \n");
    }
    void operator()(const std::vector<std::string>& args)
    {
        if (args.size() < 2) {
            usage(args[0]);
            return ;
        }

        if (args[1] == "cpu") {
            show_cpu();
        } else if (args[1] == "version") {
            show_version();
        } else {
            usage(args[0]);
        }
    }
};


class Cmd_port : public ssnlib::Command {
    System* sys;
    void show()
    {
        for (auto& port : sys->ports) {
            port.stats.update();
            port.link.update();

            auto& link = port.link;
            printf("%s\n", port.name.c_str());
            printf("  HWaddr %s %s\n", port.addr.toString().c_str(),
                    port.is_promiscuous()?"PROMISC":"NOPROMISC");
            printf("  LinkState: %s-%u-%s ",
                    link.raw.link_status==1?"Up":"Down",
                    link.raw.link_speed,
                    link.raw.link_duplex==1?"FD":"HD"
            );
            printf("\n");

            auto& stats = port.stats;
            printf("  RX packets:%lu errors:%lu dropped:%lu allocmiss:%lu \n",
                        stats.raw.ipackets, stats.raw.ierrors,
                        stats.raw.imissed, stats.raw.rx_nombuf);
            printf("  TX packets:%lu errors:%lu  \n",
                    stats.raw.opackets, stats.raw.oerrors);
            printf("  RX bytes:%lu TX bytes:%lu \n", stats.raw.ibytes, stats.raw.obytes);

            size_t nb_rxq = port.rxq.size();
            size_t nb_txq = port.txq.size();
            for (uint8_t qid=0; qid<nb_rxq; qid++) {
                printf("  RX%u packets:%lu errors:%lu ", qid,
                        stats.raw.q_ipackets[qid], stats.raw.q_errors[qid]);
                size_t rxqsize  = port.rxq[qid].size();
                size_t rxqcount = port.rxq[qid].count();
                printf("  RX ring%u:%zd/%zd \n", qid,
                        rxqcount, rxqsize);
            }
            for (uint8_t qid=0; qid<nb_txq; qid++) {
                printf("  TX%u packets:%lu ", qid, stats.raw.q_opackets[qid]);
                printf("  TX ring%u:%zd/%zd \n", qid,
                        port.txq[qid].count(), port.txq[qid].size());
            }
        }
    }
    void configure(const std::vector<std::string>& args)
    {
        if (args.size() < 2) {
            printf("Usage: %s portid\n", args[0].c_str());
            return;
        }

        size_t id = atoi(args[1].c_str());

        printf("configuring port%zd... \n", id);
        sys->ports.at(id).stop();
        sys->ports.at(id).configure();
        sys->ports.at(id).start();

    }
    void dev(const std::vector<std::string>& args)
    {
        if (args.size() < 3) {
            printf("Usage: %s portid { start | stop } \n", args[0].c_str());
            return;
        }

        size_t id = atoi(args[1].c_str());
        if (args[2] == "start") {
            sys->ports.at(id).start();
        } else if (args[2] == "stop") {
            sys->ports.at(id).stop();
        } else {
            printf("Usage: %s portid { start | stop } \n", args[0].c_str());
        }
    }
    void link(const std::vector<std::string>& args)
    {
        if (args.size() < 3) {
            printf("Usage: %s portid { up | down } \n", args[0].c_str());
            return;
        }

        size_t id = atoi(args[1].c_str());
        if (args[2] == "up") {
            sys->ports.at(id).linkup();
        } else if (args[2] == "down") {
            sys->ports.at(id).linkdown();
        } else {
            printf("Usage: %s portid { up | down } \n", args[0].c_str());
        }
    }
    void promisc(const std::vector<std::string>& args)
    {
        if (args.size() < 3) {
            printf("Usage: %s portid { on | off } \n", args[0].c_str());
            return;
        }

        size_t id = atoi(args[1].c_str());
        if (args[2] == "on") {
            sys->ports.at(id).promiscuous_set(true);
        } else if (args[2] == "off") {
            sys->ports.at(id).promiscuous_set(false);
        } else {
            printf("Usage: %s portid { on | off } \n", args[0].c_str());
        }
    }
    void ring(const std::vector<std::string>& args)
    {
        if (args.size() < 2) {
            printf("Usage: %s nub_rings \n", args[0].c_str());
            return;
        }

        size_t nb_rings = atoi(args[1].c_str());
        Port::nb_rx_rings    = nb_rings;
        Port::nb_tx_rings    = nb_rings;
    }
    void usage()
    {
        fprintf(stderr, "Usage: %s { show | configure | dev | link | promisc | ring }\n",
            name.c_str());
    }
public:
    Cmd_port(const char* n, System* s) : ssnlib::Command(n), sys(s) {}
    void operator()(const std::vector<std::string>& args)
    {
        if (args.size() < 2) {
            usage();
            return;
        }

        if (args[1] == "show") {
            show();
        } else if (args[1] == "configure") {
            std::vector<std::string> vec;
            std::copy(args.begin()+1, args.end(), std::back_inserter(vec));
            configure(vec);
        } else if (args[1] == "promisc") {
            std::vector<std::string> vec;
            std::copy(args.begin()+1, args.end(), std::back_inserter(vec));
            promisc(vec);
        } else if (args[1] == "dev") {
            std::vector<std::string> vec;
            std::copy(args.begin()+1, args.end(), std::back_inserter(vec));
            dev(vec);
        } else if (args[1] == "link") {
            std::vector<std::string> vec;
            std::copy(args.begin()+1, args.end(), std::back_inserter(vec));
            link(vec);
        } else if (args[1] == "ring") {
            std::vector<std::string> vec;
            std::copy(args.begin()+1, args.end(), std::back_inserter(vec));
            ring(vec);
        } else {
            usage();
        }
    }
};

class Cmd_thread : public  ssnlib::Command {
    System* sys;
public:
    Cmd_thread(const char* n, System* s) : ssnlib::Command(n), sys(s) {}
    void launch(size_t lcore_id)
    {
        rte_lcore_state_t state = sys->cpus[lcore_id].get_state();
        if (state == RUNNING) {
            fprintf(stderr, "Error: lcore%zd was already launched \n", lcore_id);
            return ;
        }
        sys->cpus[lcore_id].launch();
    }
    void kill(size_t lcore_id)
    {
        rte_lcore_state_t state = sys->cpus[lcore_id].get_state();
        if (state != RUNNING) {
            fprintf(stderr, "Error: lcore%zd is not runnning \n", lcore_id);
            return ;
        }
        bool ret = sys->cpus[lcore_id].thread->kill();
        if (ret) {
            rte_eal_wait_lcore(lcore_id);
            printf("done \n");
        } else {
            printf("can't killed \n");
        }
    }
    void show()
    {
        for (Cpu& cpu : sys->cpus) {
            if (cpu.lcore_id == 0) {
                printf("lcore%u thread status: COM \n", cpu.lcore_id);
            } else {
                printf("lcore%u thread status: %s \n", cpu.lcore_id,
                    ssnlib::util::rte_lcore_state_t2str(rte_eal_get_lcore_state(cpu.lcore_id)));
            }
        }
    }
    void usage(const std::string s)
    {
        fprintf(stderr, "Usage: %s { launch id | kill id | show } \n", s.c_str());
    }
    void operator()(const std::vector<std::string>& args)
    {
        if (args.size() >= 3) {
            uint8_t lcore_id = atoi(args[2].c_str());
            if (args[1] == "launch") {
                launch(lcore_id);
            } else if (args[1] == "kill") {
                kill(lcore_id);
            } else {
                usage(args[0]);
            }
            return ;
        } else if (args.size() >= 2) {
            if (args[1] == "show") {
                show();
                return ;
            }
        }
        usage(args[0]);
        return ;
    }

};





