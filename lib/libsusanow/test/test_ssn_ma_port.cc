
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
 * @file   example_ssn_ma_port.cc
 * @brief  ssn_ma_port module example
 * @author Hiroki SHIROKURA
 * @date   2017.9.24
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

bool l2fwd_running = true;
void l2fwd(void* acc_id_)
{
  size_t aid = *((size_t*)acc_id_);
  ssn_log(SSN_LOG_INFO, "start new thread %s, access_id=%zd\n", __func__, aid);

  size_t nb_ports = ssn_dev_count();
  while (l2fwd_running) {
    for (size_t pid=0; pid<nb_ports; pid++) {
      rte_mbuf* mbufs[32];

      size_t nb_recv = ssn_ma_port_rx_burst(pid, aid, mbufs, 32);
      if (nb_recv == 0) continue;

      for (size_t i=0; i<nb_recv; i++) {

        /* Delay Block begin */
        size_t n=10;
        for (size_t j=0; j<100; j++) n++;

        size_t nb_send = ssn_ma_port_tx_burst(pid^1, aid, &mbufs[i], 1);
        if (nb_send != 1)
          rte_pktmbuf_free(mbufs[i]);
      }
    }
  } /* while */

  ssn_log(SSN_LOG_INFO, "finish thread %s \n", __func__);
}

void INIT(int argc, char** argv, size_t n_que)
{
  ssn_init(argc, argv);
  rte_mempool* mp = dpdk::mp_alloc("ssn", 0, 8192);

  size_t n_ports = ssn_dev_count();
  if (n_ports != 2) throw slankdev::exception("num ports is not 2");
  for (size_t i=0; i<n_ports; i++) {
    ssn_ma_port_configure_hw(i, n_que, n_que, mp);
    ssn_ma_port_dev_up(i);
    ssn_ma_port_promisc_on(i);
  }

  if (n_ports != 2) {
    std::string err = slankdev::format("num ports is not 2 (current %zd)",
        ssn_dev_count());
    throw slankdev::exception(err.c_str());
  }
}

static void waitmsg(const char* msg)
{
  printf("\n\n");
  printf("press [Enter]...: %s ", msg);
  getchar();
}

int main(int argc, char** argv)
{
  uint32_t tid[4];
  INIT(argc, argv, 4);
  constexpr size_t gt_lcore_id = 1;
  ssn_green_thread_sched_register(gt_lcore_id);

  waitmsg("waiting deploy with 1 threads");
  ssn_ma_port_configure_acc(0, 1, 1);
  ssn_ma_port_configure_acc(1, 1, 1);
  l2fwd_running = true;
  tid[0] = ssn_thread_launch(l2fwd, &num[0], 1);

  waitmsg("waiting redeploy with 2 threads");
  l2fwd_running = false;
  ssn_thread_join(tid[0]);
  ssn_ma_port_configure_acc(0, 2, 2);
  ssn_ma_port_configure_acc(1, 2, 2);
  l2fwd_running = true;
  tid[0] = ssn_thread_launch(l2fwd, &num[0], 1);
  tid[1] = ssn_thread_launch(l2fwd, &num[1], 2);

  waitmsg("waiting redeploy with 4 threads");
  l2fwd_running = false;
  ssn_thread_join(tid[0]);
  ssn_thread_join(tid[1]);
  ssn_ma_port_configure_acc(0, 4, 4);
  ssn_ma_port_configure_acc(1, 4, 4);
  l2fwd_running = true;
  tid[0] = ssn_thread_launch(l2fwd, &num[0], gt_lcore_id);
  tid[1] = ssn_thread_launch(l2fwd, &num[1], gt_lcore_id);
  tid[2] = ssn_thread_launch(l2fwd, &num[2], gt_lcore_id);
  tid[3] = ssn_thread_launch(l2fwd, &num[3], gt_lcore_id);

  waitmsg("waiting finish");
  l2fwd_running = false;
  ssn_thread_join(tid[0]);
  ssn_thread_join(tid[1]);
  ssn_thread_join(tid[2]);
  ssn_thread_join(tid[3]);
  ssn_fin();
}

void main_neo(int argc, char** argv)
{
  constexpr size_t n_rxq = 4;
  constexpr size_t n_txq = 4;
  constexpr size_t n_ports_want = 1;

  ssn_init(argc, argv);
  rte_mempool* mp = dpdk::mp_alloc("ssn_neo", 0, 8192);
  size_t n_ports = ssn_dev_count();
  if (n_ports != n_ports_want) {
    std::string err = slankdev::format("n_ports is not %zd (current %zd)",
        n_ports_want, n_ports);
    throw slankdev::exception(err);
  }
  for (size_t i=0; i<n_ports; i++) {
    ssn_ma_port_configure_hw(i, n_rxq, n_txq, mp);
    ssn_ma_port_dev_up(i);
    ssn_ma_port_promisc_on(i);
  }

  ssn_ma_port_configure_acc(0, 2, 2);
}


