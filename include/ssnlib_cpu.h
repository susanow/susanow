

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



class Cpu {
public:
	const uint8_t lcore_id;
    const std::string name;
    Fthread* thread;

	Cpu(size_t lid) :
        lcore_id(lid),
        name("lcore" + std::to_string(lcore_id)),
        thread(nullptr)
    {
        if (lid >= rte_lcore_count()) {
            throw slankdev::exception("invalid lcore id");
        }
        kernel_log("Construct %s\n", name.c_str());
    }
    ~Cpu() { kernel_log("Dustruct %s\n", name.c_str()); }
};



} /* namespace ssnlib */

