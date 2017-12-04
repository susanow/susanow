
/*
 * MIT License
 *
 * Copyright (c) 2017 Hiroki SHIROKURA
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
 * @file   example_ssn_port.cc
 * @brief  ssn_prot module example
 * @author Hiroki SHIROKURA
 * @date   2017.9.20
 */

#include <stdio.h>
#include <unistd.h>
#include <vector>
#include <stdio.h>
#include <unistd.h>
#include <ssn_timer.h>
#include <ssn_port.h>
#include <ssn_green_thread.h>
#include <ssn_native_thread.h>
#include <ssn_port.h>
#include <ssn_timer.h>
#include <ssn_port_stat.h>
#include <ssn_cpu.h>
#include <ssn_common.h>
#include <dpdk/dpdk.h>


int main(int argc, char** argv)
{
  ssn_init(argc, argv);
  rte_mempool* mp = dpdk::mp_alloc("ssn", 0);

  ssn_port_conf conf;
  size_t nb_ports = ssn_dev_count();

  printf("---------------------------------------\n");
  for (size_t i=0; i<nb_ports; i++) {
    ssn_port_configure(i, &conf, mp);
    ssn_port_dev_up(i);
    ssn_port_link_up(i);
    ssn_port_promisc_on(i);
  }
  printf("---------------------------------------\n");
  for (size_t i=0; i<nb_ports; i++) {
    ssn_port_configure(i, &conf, mp);
    ssn_port_dev_up(i);
    ssn_port_link_up(i);
    ssn_port_promisc_on(i);
  }
  printf("---------------------------------------\n");

  ssn_fin();
}


