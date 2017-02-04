

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
 * @file ssnlib_cpu.h
 * @brief muluticore library
 * @author slankdev
 */

#pragma once
#include <string>

#include <ssnlib_log.h>
#include <ssnlib_mempool.h>
#include <ssnlib_thread.h>



namespace ssnlib {



class Cpu_interface {
private:
    static int Exe(void* arg)
    {
        ssnlib::Cpu_interface* cpu = reinterpret_cast<ssnlib::Cpu_interface*>(arg);
        (*cpu->thread)();
        return 0;
    }

public:
	const uint8_t lcore_id;
    const std::string name;
    ssn_thread* thread;

	Cpu_interface(size_t lid) :
        lcore_id(lid),
        name("lcore" + std::to_string(lcore_id)),
        thread(nullptr)
    {
        if (lid >= rte_lcore_count()) {
            throw slankdev::exception("invalid lcore id");
        }

        kernel_log(SYSTEM, "boot  %s ... done\n", name.c_str());
    }
    ~Cpu_interface() { rte_eal_wait_lcore(lcore_id); }
	void launch()
	{
        if (thread) {
            if (lcore_id == 0) {
                fprintf(stderr, "This is COM core. can not launch thread");
            } else {
                kernel_log(SYSTEM, "%s lanching ... ", name.c_str());
                rte_eal_remote_launch(Cpu_interface::Exe, this, lcore_id);
                printf("done \n");
            }
        }
	}
    rte_lcore_state_t get_state()
    {
        return rte_eal_get_lcore_state(lcore_id);
    }
};



} /* namespace ssnlib */

