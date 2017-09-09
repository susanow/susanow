

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

size_t num0 = 0;
size_t num1 = 1;
size_t num2 = 2;
size_t num3 = 3;
size_t num4 = 4;
size_t num5 = 5;
size_t num6 = 6;
size_t num7 = 7;
size_t num8 = 8;
bool block = false;

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

bool is_power_of2(size_t num)
{
  while (num != 1) {
    int ret = num % 2;
    if (ret == 0) {
      num = num/2;
      continue;
    } else {
      return false;
    }
  }
  return true;
}

void get_config(std::vector<thread_conf>& confs, size_t nb_cores, size_t nb_ports, size_t nb_ques)
{
  if (!is_power_of2(nb_cores))
    throw slankdev::exception("nb_lcore is not support");

  block = true;

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
  block = false;
}

void imple(void* arg)
{
  size_t lid = *reinterpret_cast<size_t*>(arg);

  printf("\n----------------------------\n");
  ssn_log(SSN_LOG_INFO, "start new thread \n");
  print_all_thread_conf();
  printf("----------------------------\n\n");

  size_t nb_ports = ssn_dev_count();
  while (true) {
    if (block) {
      printf("block\n");
      continue;
    }

    for (size_t pid=0; pid<nb_ports; pid++) {
      rte_mbuf* mbufs[32];
      size_t nb_recv = confs[lid].ports[pid].rx_burst(mbufs, 32);
      if (nb_recv == 0) continue;

      for (size_t i=0; i<nb_recv; i++) {
        printf("recv %zd:%zd hash=0x%x\n", pid,
            confs[lid].ports[pid].rxques_idx,
            mbufs[i]->hash.rss);
        // dpdk::hexdump_mbuf(stdout, mbufs[i]);
      }

      size_t nb_send = confs[lid].ports[pid^1].tx_burst(mbufs, nb_recv);
      if (nb_send < nb_recv)
        ssn_mbuf_free_bulk(&mbufs[nb_send], nb_recv-nb_send);
    }
    ssn_yield();
  }
  ssn_log(SSN_LOG_INFO, "finish rx-thread \n");
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

  /* 1 thread */
  getchar();
  get_config(confs, 1, nb_ports, nb_queues_per_port);
  ssn_green_thread_launch(imple, &num0, 1);

  /* 2 thread */
  getchar();
  get_config(confs, 2, nb_ports, nb_queues_per_port);
  ssn_green_thread_launch(imple, &num1, 1);

  /* 4 thread */
  getchar();
  get_config(confs, 4, nb_ports, nb_queues_per_port);
  ssn_green_thread_launch(imple, &num2, 1);
  ssn_green_thread_launch(imple, &num3, 1);

#if 0
  /* 8 thread */
  getchar();
  get_config(confs, 8,8);
  ssn_green_thread_launch(imple, &num4, 1);
  ssn_green_thread_launch(imple, &num5, 1);
  ssn_green_thread_launch(imple, &num6, 1);
  ssn_green_thread_launch(imple, &num7, 1);
#endif

  getchar();

  /*-FINI-BEGIN--------------------------------------------------------------*/
  ssn_green_thread_sched_unregister(1);
  ssn_wait_all_lcore();
  ssn_fin();
  /*-FINI-END----------------------------------------------------------------*/
}




