

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
size_t nb_rxq = 4;
size_t nb_txq = 4;

struct que {
  size_t pid;
  size_t qid;
};

struct port {
  size_t pid;
  std::vector<size_t> ques;
  size_t rx_burst(rte_mbuf* mbufs, size_t nb_bursts);
  size_t tx_burst(rte_mbuf* mbufs, size_t nb_bursts);
};

size_t port::rx_burst(rte_mbuf* mbufs, size_t nb_bursts)
{
  return 0;
}
size_t port::tx_burst(rte_mbuf* mbufs, size_t nb_bursts)
{
  return 0;
}


class thread_conf final {
 public:
  size_t lcore_id;
  std::vector<port> rx_port_range;
  std::vector<port> tx_port_range;

  virtual ~thread_conf() {}
  void clear()
  {
    lcore_id = 0;
    rx_port_range.clear();
    tx_port_range.clear();
  }
  void dump(FILE* fp) const
  {
    fprintf(fp, "  lcore_id=%zd \n", lcore_id);

    fprintf(fp, "  rx_por_range={\n");
    for (size_t pid=0; pid<rx_port_range.size(); pid++) {
      fprintf(fp, "    port%zd\n", rx_port_range[pid].pid);
      for (size_t qid=0; qid<rx_port_range[pid].ques.size(); qid++) {
        fprintf(fp, "      queue%zd\n", rx_port_range[pid].ques[qid]);
      }
    }
    fprintf(fp, "  }\n");

    fprintf(fp, "  tx_por_range={\n");
    for (size_t pid=0; pid<tx_port_range.size(); pid++) {
      fprintf(fp, "    port%zd\n", tx_port_range[pid].pid);
      for (size_t qid=0; qid<tx_port_range[pid].ques.size(); qid++) {
        fprintf(fp, "      queue%zd\n", tx_port_range[pid].ques[qid]);
      }
    }
    fprintf(fp, "  }\n");
  }
};
std::vector<thread_conf> confs;
void print_all_thread_conf();

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

void get_config(std::vector<thread_conf>& confs, size_t nb_cores, size_t nb_ques)
{
  // nb_cores = 3;

  if (!is_power_of2(nb_cores))
    throw slankdev::exception("nb_lcore is not support");

  confs.clear();
  confs.resize(nb_cores);
  const size_t nb_ports = 2;
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
      confs[i].rx_port_range.push_back(p);
      confs[i].tx_port_range.push_back(p);
    }
    que_idx += nb_ques_per_core;
  }
}

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

void imple(void* arg)
{
  size_t lid = *reinterpret_cast<size_t*>(arg);
  thread_conf* conf = &(confs[lid]);

  printf("\n----------------------------\n");
  ssn_log(SSN_LOG_INFO, "start new thread \n");
  print_all_thread_conf();
  printf("----------------------------\n\n");

  size_t nb_ports = ssn_dev_count();
  while (true) {
#if 1
    for (size_t pid=0; pid<nb_ports; pid++) {
      printf("thread polling rxq=\n");
      ssn_sleep(1000);
    }
#else
    for (size_t pid=0; pid<nb_ports; pid++) {
      rte_mbuf* mbufs[32];
      size_t nb_recv = ssn_port_rx_burst(pid, 0, mbufs, 32);
      size_t nb_recv = conf->port(pid, 0, mbufs, 32);
      if (nb_recv == 0) continue;

      size_t nb_send = ssn_port_tx_burst(pid^1,0, mbufs, nb_recv);
      if (nb_send < nb_recv)
        ssn_mbuf_free_bulk(&mbufs[nb_send], nb_recv-nb_send);
    }
#endif
    ssn_yield();
  }
  ssn_log(SSN_LOG_INFO, "finish rx-thread \n");
}

int main(int argc, char** argv)
{
  /*-BOOT-BEGIN--------------------------------------------------------------*/
  ssn_log_set_level(SSN_LOG_DEBUG);
  ssn_init(argc, argv);
  ssn_green_thread_sched_register(1);

  size_t nb_ports = ssn_dev_count();
  if (nb_ports != 2) throw slankdev::exception("num ports is not 2");
  /*-BOOT-END----------------------------------------------------------------*/

#if 0
  port p;

  /*
   * Num of Threads is 1
   */
  getchar();
  confs.resize(1);

  confs[0].clear();
  confs[0].lcore_id = 0;
  p.pid = 0; p.ques = {0,1,2,3}; confs[0].rx_port_range.push_back(p);
  p.pid = 1; p.ques = {0,1,2,3}; confs[0].rx_port_range.push_back(p);
  p.pid = 0; p.ques = {0,1,2,3}; confs[0].tx_port_range.push_back(p);
  p.pid = 1; p.ques = {0,1,2,3}; confs[0].tx_port_range.push_back(p);

  ssn_green_thread_launch(imple, &num0, 1);

  /*
   * Num of Threads is 2
   */
  getchar();
  confs.resize(2);

  confs[0].clear();
  confs[0].lcore_id = 0;
  p.pid = 0; p.ques = {0,1};     confs[0].rx_port_range.push_back(p);
  p.pid = 1; p.ques = {0,1};     confs[0].rx_port_range.push_back(p);
  p.pid = 0; p.ques = {0,1};     confs[0].tx_port_range.push_back(p);
  p.pid = 1; p.ques = {0,1};     confs[0].tx_port_range.push_back(p);

  confs[1].clear();
  confs[1].lcore_id = 1;
  p.pid = 0; p.ques = {2,3};     confs[1].rx_port_range.push_back(p);
  p.pid = 1; p.ques = {2,3};     confs[1].rx_port_range.push_back(p);
  p.pid = 0; p.ques = {2,3};     confs[1].tx_port_range.push_back(p);
  p.pid = 1; p.ques = {2,3};     confs[1].tx_port_range.push_back(p);

  ssn_green_thread_launch(imple, &num1, 1);

  /*
   * Num of Threads is 4 (LAST)
   */
  getchar();
  confs.resize(4);

  confs[0].clear();
  confs[0].lcore_id = 0;
  p.pid = 0; p.ques = {0};     confs[0].rx_port_range.push_back(p);
  p.pid = 1; p.ques = {0};     confs[0].rx_port_range.push_back(p);
  p.pid = 0; p.ques = {0};     confs[0].tx_port_range.push_back(p);
  p.pid = 1; p.ques = {0};     confs[0].tx_port_range.push_back(p);

  confs[1].clear();
  confs[1].lcore_id = 1;
  p.pid = 0; p.ques = {1};     confs[1].rx_port_range.push_back(p);
  p.pid = 1; p.ques = {1};     confs[1].rx_port_range.push_back(p);
  p.pid = 0; p.ques = {1};     confs[1].tx_port_range.push_back(p);
  p.pid = 1; p.ques = {1};     confs[1].tx_port_range.push_back(p);

  confs[2].clear();
  confs[2].lcore_id = 2;
  p.pid = 0; p.ques = {2};     confs[2].rx_port_range.push_back(p);
  p.pid = 1; p.ques = {2};     confs[2].rx_port_range.push_back(p);
  p.pid = 0; p.ques = {2};     confs[2].tx_port_range.push_back(p);
  p.pid = 1; p.ques = {2};     confs[2].tx_port_range.push_back(p);

  confs[3].clear();
  confs[3].lcore_id = 3;
  p.pid = 0; p.ques = {3};     confs[3].rx_port_range.push_back(p);
  p.pid = 1; p.ques = {3};     confs[3].rx_port_range.push_back(p);
  p.pid = 0; p.ques = {3};     confs[3].tx_port_range.push_back(p);
  p.pid = 1; p.ques = {3};     confs[3].tx_port_range.push_back(p);

  ssn_green_thread_launch(imple, &num2, 1);
  ssn_green_thread_launch(imple, &num3, 1);
#else
  getchar();
  get_config(confs, 1,4);
  ssn_green_thread_launch(imple, &num0, 1);

  getchar();
  get_config(confs, 2,4);
  ssn_green_thread_launch(imple, &num1, 1);

  getchar();
  get_config(confs, 4,4);
  ssn_green_thread_launch(imple, &num2, 1);
  ssn_green_thread_launch(imple, &num3, 1);
#endif


  /*-FINI-BEGIN--------------------------------------------------------------*/
  ssn_green_thread_sched_unregister(1);
  ssn_wait_all_lcore();
  ssn_fin();
  /*-FINI-END----------------------------------------------------------------*/
}




