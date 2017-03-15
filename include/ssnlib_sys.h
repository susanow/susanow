
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
#include <ssnlib_cpu.h>
#include <ssnlib_port.h>

#include <slankdev/exception.h>


namespace ssnlib {




int _thread_launch(void* arg)
{
    Thread* thread = reinterpret_cast<Thread*>(arg);
    uint8_t lcore_id = rte_lcore_id();
    printf("launch to lcore%u \n", lcore_id);
    thread->impl();
    return 0;
}




class System {
public:
	std::vector<Cpu>  cpus;
	std::vector<Port> ports;
    Thread_pool  threadpool;
    Lthread_pool lthreadpool;
    vty_thread    vty;
    lthread_sched ltsched;

	System(int argc, char** argv) : vty(this), ltsched(lthreadpool)
    {
        kernel_log("[+] System Boot...\n");
        int ret = rte_eal_init(argc, argv);
        if (ret < 0) {
            throw slankdev::exception("rte_eal_init");
        }

        size_t nb_cores = rte_lcore_count();
        size_t nb_ports = rte_eth_dev_count();
        kernel_log("Found %zd lcores and %zd ports\n", nb_cores, nb_ports);

        cpus.reserve(nb_cores);
        ports.reserve(nb_ports);
        for (size_t lid=0; lid<nb_cores; lid++) cpus.emplace_back(lid);
        for (size_t pid=0; pid<nb_ports; pid++) ports.emplace_back(pid);

        if (nb_cores < 2)
            throw slankdev::exception("Susanow needs at least 2 cores");
    }
    ~System()
    {
        kernel_log("[+] System Halt ...\n");
    }


    /*
     * Timer Function Interface
     * TODO: SLANKDEV
     */
    void add_timerfunc() {}

    void cyclic_task()
    {
        for (Port& port : ports) {
            port.stats.update();
        }
    }
    void dispatch()
    {
        rte_eal_remote_launch(_thread_launch, &vty    , 1);
        rte_eal_remote_launch(_thread_launch, &ltsched, 2);
    }
};




} /* namespace ssnlib */


