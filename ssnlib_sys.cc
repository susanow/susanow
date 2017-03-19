


/*-
 * MIT License
 *
 * Copyright (c) 2017 Susanow
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
 * @file ssnlib_sys.cc
 * @author slankdev
 */


#include <ssnlib_sys.h>

#include <stdio.h>
#include <assert.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <stddef.h>

#include <vector>
#include <string>

#include <slankdev/dpdk_header.h>
#include <slankdev/exception.h>

#include <ssnlib_cpu.h>
#include <ssnlib_port.h>
#include <ssnlib_thread.h>





int _thread_launch(void* arg)
{
    Fthread* thread = reinterpret_cast<Fthread*>(arg);
    uint32_t lcoreid = rte_lcore_id();
    printf("Launch thread \"%s\" to lcoreid=%u \n", thread->name.c_str(), lcoreid);
    thread->impl();
    return 0;
}


void _timer_launch(struct rte_timer *, void *arg)
{
    Tthread* tthread = reinterpret_cast<Tthread*>(arg);
    tthread->impl();
}


System::System(int argc, char** argv) : vty(this), ltsched(lthreadpool)
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

System::~System()
{
    kernel_log("[+] System Halt ...\n");
    rte_eal_mp_wait_lcore();
}

void System::timerinit()
{
    rte_timer_subsystem_init();

    if (tthreadpool.size() > MAXTIMEROBJ) {
        throw slankdev::exception("Too many timer-obj");
    }

    for (size_t i=0; i<tthreadpool.size(); i++) {
        rte_timer_init(&timer[i]);
    }

    uint64_t hz = rte_get_timer_hz();
    for (size_t i=0; i<tthreadpool.size(); i++) {
        printf("Tthread reset lcore%u \"%s\"\n", LTHRED_LCOREID, tthreadpool.get_thread(i)->name.c_str());
        rte_timer_reset(
                &timer[i], hz, PERIODICAL,
                LTHRED_LCOREID,
                _timer_launch,
                tthreadpool.get_thread(i)
        );
    }
}

void System::dispatch()
{
    timerinit();
    launch_Lthread();
    launch_Fthread(&vty);
}

void System::launch_Lthread()
{
    cpus[LTHRED_LCOREID].thread = &ltsched;
    rte_eal_remote_launch(_thread_launch, &ltsched, LTHRED_LCOREID);
}

void System::launch_Fthread(Fthread* thread)
{
    const size_t nb_cpus = cpus.size();
    for (size_t i=2; i<nb_cpus; i++) {
        if (cpus[i].thread == thread) {
            printf("Thread \"%s\" was already launched lcore%zd\n",
                                            thread->name.c_str(), i);
            return;
        }

        rte_lcore_state_t s = rte_eal_get_lcore_state(i);
        if (s == WAIT) {
            cpus[i].thread = thread;
            rte_eal_remote_launch(_thread_launch, thread, i);
            return ;
        }
    }
    printf("Can't launch Fthread: reason=no wait cpus \n");
}

void System::kill_Fthread(Fthread* thread)
{
    int64_t lcore_id = -1;
    for (Cpu& c : cpus) {
        if (c.thread == thread) {
            lcore_id = c.lcore_id;
            c.thread = nullptr;
        }
    }

    if (lcore_id == -1) {
        printf("not found thread\n");
        return ;
    }

    thread->kill();
    rte_eal_wait_lcore(lcore_id);
}





