
/*-
 * MIT License
 *
 * Copyright (c) 2017 Susanoo G
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
/**
 * @mainpage API
 *
 *
 * Susanow has many libraries thar wraped dpdk-apis and screch components:
 *      - @b SusanowSystem:
 *        ,@ref ssnlib_sys.h        "system"
 *      - @b CpuManagement:
 *         @ref ssnlib_cpu.h        "cpu"
 *        ,@ref ssnlib_thread.h     "thread"
 *      - @b PortManagement:
 *         @ref ssnlib_port.h       "port"
 *        ,@ref ssnlib_port_impl.h  "port impl"
 *        ,@ref ssnlib_mempool.h    "mempool"
 *        ,@ref ssnlib_ring.h       "ring"
 *      - @b Misc:
 *         @ref ssnlib_cfg.h        "config file"
 *        ,@ref ssnlib_cmd.h        "command"
 *        ,@ref ssnlib_dpdk.h       "dpdk util"
 *        ,@ref ssnlib_log.h        "log"
 *        ,@ref ssnlib_misc.h       "misc"
 *        ,@ref ssnlib_shell.h      "shell"
 *
 */
/**
 * @file ssnlib_sys.h
 * @brief susanow lib top header
 * @author slankdev
 */



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
	virtual void wait_all()   { rte_eal_mp_wait_lcore(); }
    virtual void launch_all() { for (CPU& cpu : cpus) cpu.launch(); }
    virtual void append_thread(ssn_thread* thread)
    {
        for (CPU& cpu : cpus) {
            if (cpu.lcore_id == 0) continue;

            if (cpu.thread == nullptr) {
                cpu.thread = thread;
                return ;
            }
        }
        throw slankdev::exception("No such lcore to append thread");
    }
    virtual void cyclic_task()
    {
        for (PORT& port : ports) {
            port.stats.update();
        }
    }
};




} /* namespace ssnlib */


