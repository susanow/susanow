
/*
 * MIT License
 *
 * Copyright (c) 2017 Susanow
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

#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <thread>

#include <ssn_nfvi.h>
#include <ssn_vnf_l2fwd1b.h>
#include <ssn_vnf_l2fwd2b.h>
#include <ssn_rest_api.h>


extern bool _ssn_system_running_flag;
void watch()
{
  constexpr size_t n_port = 2;
  size_t cnt = 0;
  while (_ssn_system_running_flag) {
    for (size_t i=0; i<n_port; i++) {

      rte_eth_stats stats;
      rte_eth_stats_get(i, &stats);

      // printf("port%zd \n", i);
      // printf("  stats\n");
      // printf("    ipackets : %lu \n", stats.ipackets );
      // printf("    opackets : %lu \n", stats.opackets );
      // printf("    ibytes   : %lu \n", stats.ibytes   );
      // printf("    obytes   : %lu \n", stats.obytes   );
      // printf("    imissed  : %lu \n", stats.imissed  );
      // printf("    ierrors  : %lu \n", stats.ierrors  );
      // printf("    oerrors  : %lu \n", stats.oerrors  );
      // printf("    rx_nombuf: %lu \n", stats.rx_nombuf);
    }
    // printf("----%04lx-----------------------------------\n", cnt++);
    sleep(1);
  }
}

int main(int argc, char** argv)
{
  ssn_nfvi nfvi(argc, argv);
  nfvi.vnf_register_to_catalog("l2fwd1b", ssn_vnfalloc_l2fwd1b);
  nfvi.vnf_register_to_catalog("l2fwd2b", ssn_vnfalloc_l2fwd2b);

  std::thread t(watch);

  nfvi.run(8888);
  t.join();
  printf("bye...\n");
}


