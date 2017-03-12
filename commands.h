

#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <string>

#include <slankdev/util.h>

#include <ssnlib_sys.h>
#include <ssnlib_cmd.h>


#if 1

class Cmd_findthread : public  ssnlib::Command {
    struct pcmd_findthread_params {
        cmdline_fixed_string_t cmd;
        cmdline_fixed_string_t name;
    };
    System* sys;
public:
    Cmd_findthread(System* s) : sys(s)
    {
        append_token(TOKEN_STRING(struct pcmd_findthread_params, cmd, "findthread"));
        append_token(TOKEN_STRING(struct pcmd_findthread_params, name, NULL));
        token_fin();

        init_raw();
        set_raw("findthread <threadname>\n\tFind thread-func-ptr by name");
    }
    void handle(void* p)
    {
        struct pcmd_findthread_params* param = reinterpret_cast<pcmd_findthread_params*>(p);

        std::string name = param->name;
        for (Cpu& cpu : sys->cpus) {
            if (cpu.thread) {
                if (cpu.thread->name == name) {
                    printf("%s at %p \n", cpu.thread->name.c_str(), cpu.thread);
                    return ;
                }
            }
        }
        printf("not found \n");
    }
};
class Cmd_quit : public ssnlib::Command {
    struct pcmd_get_params {
        cmdline_fixed_string_t cmd;
    };
    System* sys;
public:
    Cmd_quit(System* s) : sys(s)
    {
        append_token(TOKEN_STRING(struct pcmd_get_params, cmd, "quit"));
        token_fin();

        init_raw();
        set_raw("quit\n\tExit program");
    }
    void handle(void*) { sys->halt(); }
};
class Cmd_clear : public ssnlib::Command {
    struct pcmd_get_params {
        cmdline_fixed_string_t cmd;
    };
public:
    Cmd_clear()
    {
        append_token(TOKEN_STRING(struct pcmd_get_params, cmd, "clear"));
        token_fin();

        init_raw();
        set_raw("clear\n\tclear screen");
    }
    void handle(void*) { slankdev::clear_screen(); }
};

class Cmd_thread : public  ssnlib::Command {
    struct pcmd_thread_params {
        cmdline_fixed_string_t cmd;
        cmdline_fixed_string_t op ;
        cmdline_fixed_string_t op2;
    };
    System* sys;
public:
    Cmd_thread(System* s) : sys(s)
    {
        append_token(TOKEN_STRING(struct pcmd_thread_params, cmd, "thread"));
        append_token(TOKEN_STRING(struct pcmd_thread_params, op , NULL  ));
        append_token(TOKEN_STRING(struct pcmd_thread_params, op2, NULL  ));
        token_fin();

        init_raw();
        set_raw("thread <show|launch|kill> <op2>\n\tthread operations");
    }
    void launch(const char* op)
    {
        auto func = [](size_t id, System* sys)
        {
            rte_lcore_state_t state = sys->cpus.at(id).get_state();
            if (state == RUNNING) {
                fprintf(stderr, "Error: lcore%zd was already launched \n", id);
                return ;
            }
            sys->cpus.at(id).launch();
        };

        if (strcmp(op, "all") == 0) {
            for (Cpu& cpu : sys->cpus) {
                if (cpu.lcore_id==0) continue;
                func(cpu.lcore_id, sys);
            }
        } else {
            size_t lcore_id = atoi(op);
            func(lcore_id, sys);
        }
    }
    void kill(const char* op)
    {
        auto func = [](size_t id, System* sys)
        {
            rte_lcore_state_t state = sys->cpus.at(id).get_state();
            if (state != RUNNING) {
                fprintf(stderr, "Error: lcore%zd is not runnning \n", id);
                return ;
            }
            bool ret = sys->cpus.at(id).thread->kill();
            if (ret) {
                rte_eal_wait_lcore(id);
                printf("done \n");
            } else {
                printf("can't killed \n");
            }
        };

        if (strcmp(op, "all") == 0) {
            for (Cpu& cpu : sys->cpus) {
                if (cpu.lcore_id==0) continue;
                func(cpu.lcore_id, sys);
            }
        } else {
            size_t lcore_id = atoi(op);
            func(lcore_id, sys);
        }
    }
    void show(const char* op)
    {
        printf("%-5s %-10s %-15s %-15s\n", "cid", "status", "ptr", "name");
        auto func = [](size_t id, System* sys)
        {
            Cpu& cpu = sys->cpus.at(id);
            if (id == 0) {
                printf("%-5zd %-10s \n", id, "COM");
            } else {
                printf("%-5zd %-10s %-15p ", id,
                    ssnlib::util::rte_lcore_state_t2str(rte_eal_get_lcore_state(cpu.get_state())),
                    cpu.thread
                );
                if (cpu.thread)
                    printf("%-15s \n", cpu.thread->name.c_str());
                else
                    printf("\n");
            }
        };

        if (strcmp(op, "all") == 0) {
            for (Cpu& cpu : sys->cpus) {
                if (cpu.lcore_id==0) continue;
                func(cpu.lcore_id, sys);
            }
        } else {
            size_t lcore_id = atoi(op);
            func(lcore_id, sys);
        }
    }
    void handle(void* p)
    {
        struct pcmd_thread_params* param = reinterpret_cast<pcmd_thread_params*>(p);
        const char* opt  = param->op;
        const char* opt2 = param->op2;

        printf("thread %s %s \n", opt, opt2);
        if (strcmp(opt, "show") == 0) {
            show(opt2);
        } else if (strcmp(opt, "launch") == 0) {
            launch(opt2);
        } else if (strcmp(opt, "kill"  ) == 0) {
            kill(opt2);
        } else {
            printf("thread: Bad argments\n");
        }
    }
};

class Cmd_port : public ssnlib::Command {
    struct pcmd_port_params {
        cmdline_fixed_string_t cmd;
        cmdline_fixed_string_t op ;
        cmdline_fixed_string_t op2;
    };
    System* sys;
public:

    Cmd_port(System* s) : sys(s)
    {
        append_token(TOKEN_STRING(struct pcmd_port_params, cmd, "port"));
        append_token(TOKEN_STRING(struct pcmd_port_params, op , NULL  ));
        append_token(TOKEN_STRING(struct pcmd_port_params, op2, NULL  ));
        token_fin();

        init_raw();
        set_raw("port <COMMAND>: port operation\n"
                "   COMMAND = show | stat | configure | linkup | linkdown |\n"
                "             devstart | devstop | promiscon | promiscoff"
        );
    }
    void handle(void* p)
    {
        struct pcmd_port_params* param = reinterpret_cast<pcmd_port_params*>(p);
        std::string opt  = param->op;
        std::string opt2 = param->op2;

        if (opt2 == "all") {
            for (auto& port : sys->ports) {
                if (opt == "show") show(port.id);
                else if (opt == "stat"      ) stat     (port.id);
                else if (opt == "configure" ) configure(port.id);
                else if (opt == "linkup"    ) link     (port.id, true );
                else if (opt == "linkdown"  ) link     (port.id, false);
                else if (opt == "devstart"  ) dev      (port.id, true );
                else if (opt == "devstop"   ) dev      (port.id, false);
                else if (opt == "promiscon" ) promisc  (port.id, true );
                else if (opt == "promiscoff") promisc  (port.id, false);
                else printf("thread: Bad argments\n");
            }
        } else {
            if (opt == "show") show(std::stoi(opt2));
            else if (opt == "stat"      ) stat     (std::stoi(opt2));
            else if (opt == "configure" ) configure(std::stoi(opt2));
            else if (opt == "linkup"    ) link     (std::stoi(opt2), true );
            else if (opt == "linkdown"  ) link     (std::stoi(opt2), false);
            else if (opt == "devstart"  ) dev      (std::stoi(opt2), true );
            else if (opt == "devstop"   ) dev      (std::stoi(opt2), false);
            else if (opt == "promiscon" ) promisc  (std::stoi(opt2), true );
            else if (opt == "promiscoff") promisc  (std::stoi(opt2), false);
            else printf("thread: Bad argments\n");
        }
    }
private:
    void show(size_t id)
    {
        auto& port = sys->ports.at(id);
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
                    stats.cure.ipackets, stats.cure.ierrors,
                    stats.cure.imissed, stats.cure.rx_nombuf);
        printf("  TX packets:%lu errors:%lu  \n",
                stats.cure.opackets, stats.cure.oerrors);
        printf("  RX bytes:%lu TX bytes:%lu \n", stats.cure.ibytes, stats.cure.obytes);

        size_t nb_rxq = port.rxq.size();
        size_t nb_txq = port.txq.size();
        for (uint8_t qid=0; qid<nb_rxq; qid++) {
            printf("  RX%u packets:%lu errors:%lu ", qid,
                    stats.cure.q_ipackets[qid], stats.cure.q_errors[qid]);
            size_t rxqsize  = port.rxq[qid].size();
            size_t rxqcount = port.rxq[qid].count();
            printf("  RX ring%u:%zd/%zd \n", qid,
                    rxqcount, rxqsize);
        }
        for (uint8_t qid=0; qid<nb_txq; qid++) {
            printf("  TX%u packets:%lu ", qid, stats.cure.q_opackets[qid]);
            printf("  TX ring%u:%zd/%zd \n", qid,
                    port.txq[qid].count(), port.txq[qid].size());
        }
    }
    void stat(size_t id)
    {
        auto& port = sys->ports.at(id);
        printf("  Throughput Rx/Tx: %lu/%luMbps, %lu/%luKpps \n",
            port.stats.rx_bps/1000000, port.stats.tx_bps/1000000,
            port.stats.rx_pps/1000   , port.stats.tx_pps/1000
        );
    }
    void configure(size_t id)
    {
        printf("configuring port%zd... \n", id);
        sys->ports.at(id).stop();
        Port::nb_rx_rings    = 4;
        Port::nb_tx_rings    = 2;
        sys->ports.at(id).configure();
        sys->ports.at(id).start();
    }
    void link(size_t id, bool upflag)
    {
        if (upflag) {
            sys->ports.at(id).linkup();
        } else {
            sys->ports.at(id).linkdown();
        }
    }
    void dev(size_t id, bool startflag)
    {
        if (startflag) {
            sys->ports.at(id).start();
        } else {
            sys->ports.at(id).stop();
        }
    }
    void promisc(size_t id, bool onflag)
    {
        if (onflag) {
            sys->ports.at(id).promiscuous_set(true);
        } else {
            sys->ports.at(id).promiscuous_set(false);
        }
    }
};

class Cmd_lscpu : public ssnlib::Command {
    struct pcmd_lscpu_params {
        cmdline_fixed_string_t cmd;
    };
    System* sys;
public:
    Cmd_lscpu(System* s) : sys(s)
    {
        append_token(TOKEN_STRING(struct pcmd_lscpu_params, cmd, "lscpu"));
        token_fin();

        init_raw();
        set_raw("lscpu : cpu infos");
    }
    void handle(void*)
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
};

class Cmd_version : public ssnlib::Command {
    struct pcmd_version_params {
        cmdline_fixed_string_t cmd;
    };
    System* sys;
public:
    Cmd_version(System* s) : sys(s)
    {
        append_token(TOKEN_STRING(struct pcmd_version_params, cmd, "version"));
        token_fin();

        init_raw();
        set_raw("version : print version");
    }
    void handle(void*) { printf("Susanow version 0.0 \n"); }
};





#else
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
        shell->exe_cmd("thread launch 4");
        shell->exe_cmd("thread launch 5");
    }
};
#endif



