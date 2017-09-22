
#include <ssn_common.h>
#include <ssn_thread.h>
#include <ssn_port.h>
#include <ssn_log.h>
#include <ssn_cpu.h>
#include <ssn_port_stat.h>
#include <memory>
#include <stdio.h>
#include <unistd.h>
#include <dpdk/dpdk.h>
#include <slankdev/exception.h>
#include "lib.h"

size_t num0 = 0;
size_t num1 = 1;
size_t num2 = 2;
size_t num3 = 3;
size_t num4 = 4;
size_t num5 = 5;
size_t num6 = 6;
size_t num7 = 7;
size_t num8 = 8;

struct port {
  size_t rxques_idx;
  size_t txques_idx;

  size_t pid;
  std::vector<size_t> ques;

  port() : rxques_idx(0), txques_idx(0) {}
  size_t rx_burst(rte_mbuf** mbufs, size_t nb_bursts);
  size_t tx_burst(rte_mbuf** mbufs, size_t nb_bursts);
};

size_t port::rx_burst(rte_mbuf** mbufs, size_t nb_bursts)
{
  rxques_idx = (rxques_idx + 1) % ques.size();
  size_t nb_recv = ssn_port_rx_burst(pid, ques.at(rxques_idx), mbufs, nb_bursts);
  return nb_recv;
}
size_t port::tx_burst(rte_mbuf** mbufs, size_t nb_bursts)
{
  txques_idx = (txques_idx + 1) % ques.size();
  size_t nb_send = ssn_port_tx_burst(pid, ques.at(txques_idx), mbufs, nb_bursts);
  return nb_send;
}


class thread_conf final {
 public:
  size_t lcore_id;
  std::vector<port> ports;

  virtual ~thread_conf() {}
  void clear()
  {
    lcore_id = 0;
    ports.clear();
  }
  void dump(FILE* fp) const
  {
    fprintf(fp, "  lcore_id=%zd \n", lcore_id);

    fprintf(fp, "  por_range={\n");
    for (size_t pid=0; pid<ports.size(); pid++) {
      fprintf(fp, "    port%zd\n", ports[pid].pid);
      for (size_t qid=0; qid<ports[pid].ques.size(); qid++) {
        fprintf(fp, "      queue%zd\n", ports[pid].ques[qid]);
      }
    }
    fprintf(fp, "  }\n");
  }
};
std::vector<thread_conf> confs;

void print_all_thread_conf()
{
  printf("ALL CONFIGURATION \n");
  size_t n_confs = confs.size();
  for (size_t i=0; i<n_confs; i++) {
    printf("conf[%zd]\n", i);
    printf("{\n");
    confs[i].dump(stdout);
    printf("}\n");
  }
}
void get_config(std::vector<thread_conf>& confs, size_t nb_cores, size_t nb_ports, size_t nb_ques, size_t)
{
  if (!is_power_of2(nb_cores))
    throw slankdev::exception("nb_lcore is not support");

  confs.clear();
  confs.resize(nb_cores);
  const size_t nb_ques_per_core = nb_ques / nb_cores;

  size_t que_idx = 0;
  for (size_t i=0; i<nb_cores; i++) {
    confs[i].lcore_id = i;
    for (size_t pid=0; pid<nb_ports; pid++) {
      port p;
      p.pid = pid;
      for (size_t qid=0; qid<nb_ques_per_core; qid++) {
        p.ques.push_back(qid+que_idx);
      }
      confs[i].ports.push_back(p);
    }
    que_idx += nb_ques_per_core;
  }
}

bool running = true;
void imple(void* arg)
{
  size_t lid = *reinterpret_cast<size_t*>(arg);

  printf("\n----------------------------\n");
  ssn_log(SSN_LOG_INFO, "start new thread \n");
  print_all_thread_conf();
  printf("----------------------------\n\n");

  size_t nb_ports = ssn_dev_count();
  while (running) {
    for (size_t pid=0; pid<nb_ports; pid++) {
      rte_mbuf* mbufs[32];
      size_t nb_recv = confs[lid].ports[pid].rx_burst(mbufs, 32);
      if (nb_recv == 0) continue;

      for (size_t i=0; i<nb_recv; i++) {

        /* Delay Block begin */
        size_t n=10;
        for (size_t j=0; j<100; j++) n++;
        /* Delay Block end */

        size_t nb_send = confs[lid].ports[pid^1].tx_burst(&mbufs[i], 1);
        if (nb_send != 1)
          rte_pktmbuf_free(mbufs[i]);
      }

    }
  }
  ssn_log(SSN_LOG_INFO, "finish rx-thread \n");
}

/*
 * SSN_SYSTEM CONFIG
 */
constexpr size_t green_thread_lcore_mask  = 0x01;
constexpr size_t timer_thread_lcore_mask  = 0x02;
constexpr size_t native_thread_lcore_mask = 0xfc;
constexpr size_t n_rx_queues_per_port = 4;
constexpr size_t n_tx_queues_per_port = 4;
void INIT(int argc, char** argv)
{
  ssn_log_set_level(SSN_LOG_EMERG);
  ssn_init(argc, argv);
  ssn_green_thread_sched_register(1);

  ssn_port_conf conf;
  conf.nb_rxq = n_rx_queues_per_port;
  conf.nb_txq = n_rx_queues_per_port;
  conf.raw.rxmode.mq_mode = ETH_MQ_RX_RSS;
  conf.raw.rx_adv_conf.rss_conf.rss_key = NULL;
  conf.raw.rx_adv_conf.rss_conf.rss_hf = ETH_RSS_IP|ETH_RSS_TCP|ETH_RSS_UDP;
  conf.debug_dump(stdout);

  const size_t n_ports = ssn_dev_count();
  if (n_ports != 2) throw slankdev::exception("num ports is not 2");
  for (size_t i=0; i<n_ports; i++) {
    ssn_port_configure(i, &conf);
    ssn_port_dev_up(i);
    ssn_port_promisc_on(i);
  }

  if (n_ports != 2) {
    std::string err = slankdev::format("num ports is not 2 (current %zd)",
        ssn_dev_count());
    throw slankdev::exception(err.c_str());
  }
}

void new_main(int argc, char** argv)
{
  /*-BOOT-BEGIN--------------------------------------------------------------*/
  INIT(argc, argv);
  /*-BOOT-END----------------------------------------------------------------*/

  const size_t nb_ports = ssn_dev_count();
  uint32_t tid0,tid1,tid2,tid3;

  /* 1 thread */
  getchar();
  running = false;
  get_config(confs, 1, nb_ports, n_rx_queues_per_port, n_tx_queues_per_port);
  running = true;
  tid0 = ssn_thread_launch(imple, &num0, 2);

  /* 2 thread */
  getchar();
  running = false;
  ssn_thread_join(tid0);
  get_config(confs, 2, nb_ports, n_rx_queues_per_port, n_tx_queues_per_port);
  running = true;
  tid0 = ssn_thread_launch(imple, &num0, 2);
  tid1 = ssn_thread_launch(imple, &num1, 3);

  /* 4 thread */
  getchar();
  running = false;
  ssn_thread_join(tid0);
  ssn_thread_join(tid1);
  get_config(confs, 4, nb_ports, n_rx_queues_per_port, n_tx_queues_per_port);
  running = true;
  tid0 = ssn_thread_launch(imple, &num0, 2);
  tid1 = ssn_thread_launch(imple, &num1, 3);
  tid2 = ssn_thread_launch(imple, &num2, 4);
  tid3 = ssn_thread_launch(imple, &num3, 5);

  getchar();
  /*-FINI-BEGIN--------------------------------------------------------------*/
  ssn_green_thread_sched_unregister(1);
  ssn_wait_all_lcore();
  ssn_fin();
  /*-FINI-END----------------------------------------------------------------*/
}


int main(int argc, char** argv)
{
  constexpr size_t nb_queues_per_port = 4;

  /*-BOOT-BEGIN--------------------------------------------------------------*/
  ssn_log_set_level(SSN_LOG_EMERG);
  ssn_init(argc, argv);
  ssn_green_thread_sched_register(1);

  const size_t nb_ports = ssn_dev_count();
  if (nb_ports != 2) throw slankdev::exception("num ports is not 2");

  ssn_port_conf conf;
  conf.nb_rxq = nb_queues_per_port;
  conf.nb_txq = nb_queues_per_port;
  conf.raw.rxmode.mq_mode = ETH_MQ_RX_RSS;
  conf.raw.rx_adv_conf.rss_conf.rss_key = NULL;
  conf.raw.rx_adv_conf.rss_conf.rss_hf = ETH_RSS_IP|ETH_RSS_TCP|ETH_RSS_UDP;
  conf.debug_dump(stdout);
  for (size_t i=0; i<nb_ports; i++) {
    ssn_port_configure(i, &conf);
    ssn_port_dev_up(i);
    ssn_port_promisc_on(i);
  }
  /*-BOOT-END----------------------------------------------------------------*/

  uint32_t tid0,tid1,tid2,tid3;

  /* 1 thread */
  getchar();
  running = false;
  get_config(confs, 1, nb_ports, n_rx_queues_per_port, n_tx_queues_per_port);
  running = true;
  tid0 = ssn_thread_launch(imple, &num0, 2);

  /* 2 thread */
  getchar();
  running = false;
  ssn_thread_join(tid0);
  get_config(confs, 2, nb_ports, n_rx_queues_per_port, n_tx_queues_per_port);
  running = true;
  tid0 = ssn_thread_launch(imple, &num0, 2);
  tid1 = ssn_thread_launch(imple, &num1, 3);

  /* 4 thread */
  getchar();
  running = false;
  ssn_thread_join(tid0);
  ssn_thread_join(tid1);
  get_config(confs, 4, nb_ports, n_rx_queues_per_port, n_tx_queues_per_port);
  running = true;
  tid0 = ssn_thread_launch(imple, &num0, 2);
  tid1 = ssn_thread_launch(imple, &num1, 3);
  tid2 = ssn_thread_launch(imple, &num2, 4);
  tid3 = ssn_thread_launch(imple, &num3, 5);

  getchar();
  /*-FINI-BEGIN--------------------------------------------------------------*/
  ssn_green_thread_sched_unregister(1);
  ssn_wait_all_lcore();
  ssn_fin();
  /*-FINI-END----------------------------------------------------------------*/
}


