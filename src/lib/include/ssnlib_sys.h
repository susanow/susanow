

#pragma once

#include <stdio.h>
#include <assert.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <stddef.h>

#include <vector>
#include <string>

#include <ssnlib_thread.h>
#include <ssnlib_cmd.h>
#include <ssnlib_cpu.h>
#include <ssnlib_port.h>
#include <ssnlib_misc.h>

#include <slankdev/exception.h>


namespace ssnlib {



template <class CPU, class PORT>
class System_interface {
public:
	std::vector<CPU>  cpus;
	std::vector<PORT> ports;
    bool              cpuflags[RTE_CPUFLAG_NUMFLAGS];

	System_interface(int argc, char** argv)
    {
        /*
         * Boot DPDK System.
         */
        kernel_log(SYSTEM, "[+] Booting ...\n");
        ssnlib::print_message();

        /*
         * DPDK init
         */
        int ret = rte_eal_init(argc, argv);
        if (ret < 0) {
            throw slankdev::exception("rte_eal_init");
        }

        kernel_log(SYSTEM, "configure \n");

        size_t nb_cores = rte_lcore_count();
        for (size_t lcore_id=0; lcore_id<nb_cores; lcore_id++) cpus.emplace_back(lcore_id);

        size_t nb_ports = rte_eth_dev_count();
        for (size_t pid=0; pid<nb_ports; pid++) ports.emplace_back(pid);

        for (size_t i=0; i<RTE_CPUFLAG_NUMFLAGS; i++)
            cpuflags[i] = rte_cpu_get_flag_name(rte_cpu_flag_t(i));

        for (auto& port : ports) port.boot();

        kernel_log(SYSTEM, "[+] DPDK boot Done! \n");
    }
    virtual ~System_interface() { rte_eal_mp_wait_lcore(); }
    virtual void halt()
    {
        kernel_log(SYSTEM, "[+] System Halt ...\n");
        rte_exit(0, "Bye...\n");
    }
	virtual void wait_all()
    {
        sleep(1);
        rte_eal_mp_wait_lcore();
    }
};




} /* namespace ssnlib */


