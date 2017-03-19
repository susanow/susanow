
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

#include <slankdev/dpdk_header.h>
#include <slankdev/exception.h>



enum {
    LTHRED_LCOREID = 1,
    MAXTIMEROBJ    = 1000,
};


class System {
public:
	std::vector<Cpu>  cpus;
	std::vector<Port> ports;
    Fthread_pool fthreadpool;
    Lthread_pool lthreadpool;
    Tthread_pool tthreadpool;
    vty_thread    vty;
    lthread_sched ltsched;
    struct rte_timer timer[MAXTIMEROBJ];

	System(int argc, char** argv);
    ~System();
    void timerinit();
    void dispatch();
    void launch_Lthread();
    void launch_Fthread(Fthread* thread);
    void kill_Fthread(Fthread* thread);
};



