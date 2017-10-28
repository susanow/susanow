
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
#include <stddef.h>
#include <stdint.h>
#include <unistd.h>
#include <stdio.h>
#include <memory>

#include <ssn_port.h>
#include <ssn_log.h>
#include <ssn_cpu.h>
#include <ssn_common.h>
#include <ssn_thread.h>
#include <ssn_port_stat.h>
#include <ssn_ma_port.h>

#include <slankdev/string.h>
#include <slankdev/exception.h>
#include <dpdk/dpdk.h>


size_t num[] = {0,1,2,3,4,5,6,7,8};

void operate_packet(rte_mbuf* mbuf, size_t pid, size_t aid, size_t qid)
{
  printf("Port%zd:%zd accessid=%zd \n", pid, qid, aid);
  // dpdk::hexdump_mbuf(stdout, mbuf);
  // printf("\n");
}

bool running = true;
void packet_capture(void* acc_id_)
{
  size_t aid = *((size_t*)acc_id_);
  printf("IMPORTANT: start new thread %s, access_id=%zd\n", __func__, aid);

  size_t nb_ports = ssn_dev_count();
  while (running) {
    for (size_t pid=0; pid<nb_ports; pid++) {
      rte_mbuf* mbufs[32];
      size_t qid = ssn_ma_port_get_next_rxqid_from_aid(pid, aid);
      // printf("rxburst pid=%zd qid=%zd \n", pid, qid);
      size_t nb_recv = ssn_ma_port_rx_burst(pid, aid, mbufs, 32);
      if (nb_recv == 0) continue;
      for (size_t i=0; i<nb_recv; i++) {
        operate_packet(mbufs[i], pid, aid, qid);
        rte_pktmbuf_free(mbufs[i]);
      }
    }
    ssn_yield();
  } /* while */

  ssn_log(SSN_LOG_INFO, "finish thread %s \n", __func__);
}

int main(int argc, char** argv)
{
  constexpr size_t n_rxq = 8;
  constexpr size_t n_txq = 8;
  constexpr size_t n_rxacc = 4;
  constexpr size_t n_txacc = 4;
  constexpr size_t wanted_n_ports = 1;
  uint32_t tid[4];
  ssn_init(argc, argv);

  rte_mempool* mp = dpdk::mp_alloc("ssn");

  auto n_ports = ssn_dev_count();
  if (n_ports != wanted_n_ports) {
    std::string err = slankdev::format("num ports is not %zd (current %zd)",
        wanted_n_ports, ssn_dev_count());
    throw slankdev::exception(err.c_str());
  }

  constexpr size_t gt_lcore_id = 1;
  ssn_green_thread_sched_register(gt_lcore_id);

  for (size_t i=0; i<n_ports; i++) {
    ssn_ma_port_configure_hw(i, n_rxq, n_txq, mp);
    ssn_ma_port_dev_up(i);
    ssn_ma_port_promisc_on(i);
  }

  printf("\n\n");
  slankdev::waitmsg("press [Enter] to Deploy...");
  printf("\n\n");

  ssn_ma_port_configure_acc(0, n_rxacc, n_txacc);
  tid[0] = ssn_thread_launch(packet_capture, &num[0], gt_lcore_id);
  tid[1] = ssn_thread_launch(packet_capture, &num[1], gt_lcore_id);
  tid[2] = ssn_thread_launch(packet_capture, &num[2], gt_lcore_id);
  tid[3] = ssn_thread_launch(packet_capture, &num[3], gt_lcore_id);
  getchar();
  running = false;
  ssn_thread_join(tid[0]);
  ssn_thread_join(tid[1]);
  ssn_thread_join(tid[2]);
  ssn_thread_join(tid[3]);

  rte_mempool_free(mp);
  ssn_fin();
}


