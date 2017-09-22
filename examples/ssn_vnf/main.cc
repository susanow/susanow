
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

size_t num[] = {0,1,2,3,4,5,6,7,8};

struct p_port {
  size_t rxques_idx;
  size_t txques_idx;

  size_t pid;
  std::vector<size_t> ques;

  p_port() : rxques_idx(0), txques_idx(0) {}
  size_t rx_burst(rte_mbuf** mbufs, size_t nb_bursts)
  {
    rxques_idx = (rxques_idx + 1) % ques.size();
    size_t nb_recv = ssn_port_rx_burst(pid, ques.at(rxques_idx), mbufs, nb_bursts);
    return nb_recv;
  }
  size_t tx_burst(rte_mbuf** mbufs, size_t nb_bursts)
  {
    txques_idx = (txques_idx + 1) % ques.size();
    size_t nb_send = ssn_port_tx_burst(pid, ques.at(txques_idx), mbufs, nb_bursts);
    return nb_send;
  }
};


class thread_conf final {
 public:
  size_t lcore_id;
  std::vector<p_port> ports;

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
      p_port p;
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
void vnf_imple(void* arg)
{
  ssn_log(SSN_LOG_INFO, "start new thread \n");
  size_t lid = *reinterpret_cast<size_t*>(arg);

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

enum ssn_vnf_port_type {
  DPDK,
  VIRT,
};
const char* ssn_vnf_port_type2str(enum ssn_vnf_port_type e)
{
  switch (e) {
    case DPDK: return "DPDK";
    case VIRT: return "VIRT";
  }
}
class ssn_vnf_port {
  const size_t port_id;
  const size_t n_rx_queues;
  const size_t n_tx_queues;
  const ssn_vnf_port_type type;
 public:
  ssn_vnf_port(size_t pid, size_t ntxq, size_t nrxq) :
    port_id(pid),
    n_rx_queues(nrxq),
    n_tx_queues(ntxq), type(DPDK) {}
  void debug_dump(FILE* fp) const
  {
    /*             pid    nrxq   ntxq   type       */
    fprintf(fp, "  %-3zd  %-4zd  %-4zd  %-10s \r\n",
        port_id, n_rx_queues, n_tx_queues,
        ssn_vnf_port_type2str(type));
  }
};


class ssn_vnf;

class ssn_nfvi {
 public:
  constexpr static size_t master_thread_lcore_mask = 0x01;
  constexpr static size_t green_thread_lcore_mask  = 0x02;
  constexpr static size_t timer_thread_lcore_mask  = 0x04;
  constexpr static size_t native_thread_lcore_mask = 0xf8;
  constexpr static size_t n_rx_queues_per_port = 4;
  constexpr static size_t n_tx_queues_per_port = 4;
  std::vector<ssn_vnf_port> ports;
  std::vector<ssn_vnf*> vnfs;

  void INIT(int argc, char** argv)
  {
    ssn_init(argc, argv);

    std::vector<size_t> vec = coremask2vecor(green_thread_lcore_mask);
    auto n_lcores = vec.size();
    for (size_t i=0; i<n_lcores; i++) {
      ssn_green_thread_sched_register(vec[i]);
    }

    ssn_port_conf conf;
    conf.nb_rxq = n_rx_queues_per_port;
    conf.nb_txq = n_tx_queues_per_port;
    conf.raw.rxmode.mq_mode = ETH_MQ_RX_RSS;
    conf.raw.rx_adv_conf.rss_conf.rss_key = NULL;
    conf.raw.rx_adv_conf.rss_conf.rss_hf = ETH_RSS_IP|ETH_RSS_TCP|ETH_RSS_UDP;
    /* conf.debug_dump(stdout); */

    size_t n_ports = ssn_dev_count();
    if (n_ports != 2) throw slankdev::exception("num ports is not 2");
    for (size_t i=0; i<n_ports; i++) {
      ssn_port_configure(i, &conf);
      ssn_port_dev_up(i);
      ssn_port_promisc_on(i);

      ssn_vnf_port port(i, n_tx_queues_per_port, n_rx_queues_per_port);
      ports.push_back(port);
    }

    if (n_ports != 2) {
      std::string err = slankdev::format("num ports is not 2 (current %zd)",
          ssn_dev_count());
      throw slankdev::exception(err.c_str());
    }
  }
  void FINI()
  {
    std::vector<size_t> vec = coremask2vecor(green_thread_lcore_mask);
    auto n_lcores = vec.size();
    for (size_t i=0; i<n_lcores; i++) {
      ssn_green_thread_sched_unregister(vec[i]);
    }
    ssn_wait_all_lcore();
    ssn_fin();
  }
  void ports_debug_dump(FILE* fp)
  {
    fprintf(fp, "\r\n");
    fprintf(fp, "  %-3s  %-4s  %-4s  %-10s \r\n", "pid", "nrxq", "ntxq", "type");
    fprintf(fp, " -----------------------------------------------------\r\n");
    size_t n_ports = ports.size();
    for (size_t i=0; i<n_ports; i++) {
      ports[i].debug_dump(fp);
    }
    fprintf(fp, "\r\n");
  }
  void vnfs_debug_dump(FILE* fp);
};

class ssn_vnf {
  const size_t n_ports;
  const size_t n_rx_queues_per_port;
  const size_t n_tx_queues_per_port;
  size_t n_threads;
  size_t core_mask;
  size_t port_mask;
  std::vector<uint32_t> tids;
  ssn_nfvi* nfvi;
  ssn_function_t imple;

 public:
  ssn_vnf(ssn_nfvi* nfvi_, size_t n_threads_,
      uint32_t core_mask_, uint32_t port_mask_,
      ssn_function_t f) :
    n_ports(nfvi_->ports.size()),
    n_rx_queues_per_port(nfvi_->n_rx_queues_per_port),
    n_tx_queues_per_port(nfvi_->n_tx_queues_per_port),
    n_threads(n_threads_),
    core_mask(core_mask_),
    port_mask(port_mask_),
    nfvi(nfvi_), imple(f)
  {
    nfvi->vnfs.push_back(this);
    get_config(confs, n_threads, n_ports, n_rx_queues_per_port, n_tx_queues_per_port);
  }
  ~ssn_vnf()
  {
    size_t n_vnfs = nfvi->vnfs.size();
    for (size_t i=0; i<n_vnfs; i++) {
      if (nfvi->vnfs[i] == this) {
        nfvi->vnfs.erase(nfvi->vnfs.begin() + i);
        return ;
      }
    }
    throw slankdev::exception("OKASHIII");
  }
  void debug_dump(FILE* fp) const
  {
    /*             nports coremask portmask nthreads */
    fprintf(fp, "  %-5zd 0x%04lx 0x%04lx  %zd\r\n",
        n_ports, core_mask, port_mask, n_threads);
  }
  void undeploy()
  {
    running = false;
    for (size_t i=0; i<n_threads; i++) {
      ssn_thread_join(tids[i]);
    }
    printf("UNDEPLOY SUCCESS\n");
  }
  void showconf()
  {
    printf("\n----------------------------\n");
    print_all_thread_conf();
    printf("----------------------------\n\n");
  }
  void deploy()
  {
    std::vector<size_t> vcores = coremask2vecor(core_mask);

    for (size_t i=0; i<vcores.size(); i++) {
      printf("%zd, ", vcores[i]);
    } printf("\n");

    tids.clear();
    tids.resize(n_threads);
    running = true;

    for (size_t i=0; i<vcores.size(); i++) {
      tids[i] = ssn_thread_launch(imple, &num[i], vcores[i]);
    }
    printf("DEPLOY SUCCESS\n");
  }
};

void ssn_nfvi::vnfs_debug_dump(FILE* fp)
{
  fprintf(fp, "\r\n");
    /*             nports coremask portmask nthreads */
  fprintf(fp, "  %-5s %-6s %-6s  %s \r\n", "nport", "cmask", "pmask", "nthrd");
  fprintf(fp, "---------------------------------------------------------\r\n");
  size_t n_vnfs = vnfs.size();
  for (auto* vnf : vnfs) {
    vnf->debug_dump(stdout);
  }
  fprintf(fp, "\r\n");
}

int main(int argc, char** argv)
{
  ssn_nfvi nfvi;
  nfvi.INIT(argc, argv);
  ssn_vnf* vnf0 = nullptr;

  nfvi.ports_debug_dump(stdout);

  getchar();
  vnf0 = new ssn_vnf(&nfvi, 1, 0x04, 0x03, vnf_imple);
  nfvi.vnfs_debug_dump(stdout);
  vnf0->showconf();
  vnf0->deploy();

  getchar();
  vnf0->undeploy();
  delete vnf0;
  vnf0 = new ssn_vnf(&nfvi, 2, 0x0c, 0x03, vnf_imple);
  nfvi.vnfs_debug_dump(stdout);
  vnf0->showconf();
  vnf0->deploy();

  getchar();
  vnf0->undeploy();
  delete vnf0;
  vnf0 = new ssn_vnf(&nfvi, 4, 0x3c, 0x03, vnf_imple);
  nfvi.vnfs_debug_dump(stdout);
  vnf0->showconf();
  vnf0->deploy();

  getchar();
  nfvi.FINI();
}


