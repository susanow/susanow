

#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <unistd.h>
#include <assert.h>
#include <vector>
#include <slankdev/string.h>
#include <slankdev/exception.h>

#include <ssn_port.h>
#include <ssn_port_stat.h>
#include <ssn_ma_port.h>
#include <ssn_log.h>
#include <ssn_cpu.h>
#include <ssn_common.h>
#include <ssn_thread.h>


void INIT(int argc, char** argv)
{
  ssn_init(argc, argv);
  size_t n_ports = ssn_dev_count();
  if (n_ports != 2) {
    std::string err = slankdev::format("n_ports is not 2 (current %zd)",
        ssn_dev_count());
    throw slankdev::exception(err.c_str());
  }
}

class ssn_vnf_port {
  const size_t port_id;
  const size_t n_rxq;
  const size_t n_txq;
 public:
  ssn_vnf_port(size_t a_port_id, size_t a_n_rxq, size_t a_n_txq) :
    port_id(a_port_id), n_rxq(a_n_rxq), n_txq(a_n_txq)
  {
    ssn_ma_port_configure_hw(port_id, n_rxq, n_txq);
    ssn_ma_port_dev_up(port_id);
    ssn_ma_port_promisc_on(port_id);
  }
  void configure(size_t n_rxacc, size_t n_txacc)
  { ssn_ma_port_configure_acc(port_id, n_rxacc, n_txacc); }
  size_t tx_burst(size_t aid, rte_mbuf** mbufs, size_t n_mbufs)
  { return ssn_ma_port_tx_burst(port_id, aid, mbufs, n_mbufs); }
  size_t rx_burst(size_t aid, rte_mbuf** mbufs, size_t n_mbufs)
  { return ssn_ma_port_rx_burst(port_id, aid, mbufs, n_mbufs); }
};

class ssn_vnf {
 private:
  static void _vnf_thread_spawner(void* vnf_instance_)
  {
    ssn_vnf* vnf = reinterpret_cast<ssn_vnf*>(vnf_instance_);
    vnf->deploy_imple(nullptr);
  }
  std::vector<ssn_vnf_port*> ports;

 protected:
  std::vector<uint32_t> tids;
  std::vector<size_t> lcores;

  virtual void deploy_imple(void* acc_id_) = 0;
  virtual void undeploy_imple() = 0;
  size_t n_ports() const { return ports.size(); }
  size_t tx_burst(size_t pid, size_t aid, rte_mbuf** mbufs, size_t n_mbufs)
  { return ports[pid]->tx_burst(aid, mbufs, n_mbufs); }
  size_t rx_burst(size_t pid, size_t aid, rte_mbuf** mbufs, size_t n_mbufs)
  { return ports[pid]->rx_burst(aid, mbufs, n_mbufs); }

 public:
  ssn_vnf(size_t n_ports_a) : ports(n_ports_a) {}
  virtual ~ssn_vnf() {}

  void attach_port(size_t pid, ssn_vnf_port* p) { ports.at(pid) = p; }
  virtual void deploy(uint32_t lcore_mask)
  {
    lcores.clear();
    for (size_t i=0; i<32; i++) {
      if ((lcore_mask & (0x1<<i)) != 0)
        lcores.push_back(i);
    }
    tids.resize(lcores.size());

    for (size_t i=0; i<ports.size(); i++) {
      ports[i]->configure(lcores.size(), lcores.size());
    }

    for (size_t i=0; i<lcores.size(); i++) {
      tids[i] = ssn_thread_launch(_vnf_thread_spawner, this, lcores[i]);
    }
  }
  virtual void undeploy()
  {
    assert(tids.size() == lcores.size());

    undeploy_imple();
    for (size_t i=0; i<lcores.size(); i++) {
      ssn_thread_join(tids[i]);
    }
    printf("undeploy succes\n");
  }
  size_t get_aid_from_lcoreid(size_t lcore_id) const
  {
    size_t n_lcores = lcores.size();
    for (size_t i=0; i<n_lcores; i++) {
      if (lcores[i] == lcore_id) return i;
    }
    throw slankdev::exception(
        "vnf::get_aid_from_lcoreid: not found aid (lcoreid is invalid?)");
  }
  size_t get_aid() const
  {
    size_t lcore_id = ssn_lcore_id();
    return get_aid_from_lcoreid(lcore_id);
  }
};

class vnf_test : public ssn_vnf {
  bool l2fwd_running;
  virtual void undeploy_imple() override { l2fwd_running=false; }
  virtual void deploy_imple(void*) override
  {
    l2fwd_running = true;
    size_t aid = get_aid();
    ssn_log(SSN_LOG_INFO, "start new thread %s, access_id=%zd\n", __func__, aid);

    size_t nb_ports = n_ports();
    while (l2fwd_running) {
      for (size_t pid=0; pid<nb_ports; pid++) {
        rte_mbuf* mbufs[32];
        size_t nb_recv = rx_burst(pid, aid, mbufs, 32);
        if (nb_recv == 0) continue;

        for (size_t i=0; i<nb_recv; i++) {

          /* Delay Block begin */
          size_t n=10;
          for (size_t j=0; j<100; j++) n++;

          size_t nb_send = tx_burst(pid^1, aid, &mbufs[i], 1);
          if (nb_send != 1)
            rte_pktmbuf_free(mbufs[i]);
        }
      }
    } /* while */

    ssn_log(SSN_LOG_INFO, "finish thread %s \n", __func__);
  }
 public:
  vnf_test(size_t np) : ssn_vnf(np), l2fwd_running(true) {}
};

char waitmsg(const char* msg)
{
  printf(msg);
  return getchar();
}

int main(int argc, char** argv)
{
  INIT(argc, argv);

  const size_t n_ports = 2;
  vnf_test* vnf0 = new vnf_test(n_ports);

  ssn_vnf_port* port0 = new ssn_vnf_port(0, 4, 4); // dpdk0
  ssn_vnf_port* port1 = new ssn_vnf_port(1, 4, 4); // dpdk1
  vnf0->attach_port(0, port0); // attach dpdk0 to vnf0
  vnf0->attach_port(1, port1); // attach dpdk1 to vnf0

  /* run with 1 cores */
  waitmsg("press [enter] to deploy with 1 lcores...\n");
  vnf0->undeploy();
  vnf0->deploy(0x04);

  /* run with 2 cores */
  waitmsg("press [enter] to deploy with 2 lcores...\n");
  vnf0->undeploy();
  vnf0->deploy(0x0c);

  /* run with 4 cores */
  waitmsg("press [enter] to deploy with 4 lcores...\n");
  vnf0->undeploy();
  vnf0->deploy(0x3c);

  /* undeploy */
  waitmsg("press [enter] to undeploy ");
  vnf0->undeploy();

fin:
  delete vnf0;
  delete port0;
  delete port1;
  ssn_fin();
}

#if 0
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
void vnfs_debug_dump(FILE* fp)
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
#endif


