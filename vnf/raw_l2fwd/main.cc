
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <thread>
#include <dpdk/wrap.h>

// extern "C" {
// #include "rte_eth_pipe.h"
// }

constexpr size_t n_rxq = 8;
constexpr size_t n_txq = 8;
size_t ports[2];
bool running = true;

static void signal_hancler(int signum)
{
  if (signum == SIGINT || signum == SIGTERM) {
    running = false;
  }
}

int polling_port(void*) {
  printf("call %s() on lcore%u\n", __func__, rte_lcore_id());
  const size_t n_port = rte_eth_dev_count();
  while (running) {
    for (size_t pid_idx=0; pid_idx<n_port; pid_idx++) {
      size_t pid = ports[pid_idx];
      for (size_t qid=0; qid<n_rxq; qid++) {
        rte_mbuf* mbufs[32];
        size_t nrx = rte_eth_rx_burst(pid, qid, mbufs, 32);
        if (nrx == 0) continue;
#if 1
        size_t dstpid = ports[pid_idx^1];
        size_t ntx = rte_eth_tx_burst(pid, qid, mbufs, nrx);
        if (ntx < nrx) dpdk::rte_pktmbuf_free_bulk(&mbufs[ntx], nrx-ntx);
#else
        for (size_t i=0; i<nrx; i++) {
          uint8_t* ptr = rte_pktmbuf_mtod(mbufs[i], uint8_t*);
          ptr[0] = 0xaa;
          size_t dstpid = ports[pid_idx^1];
          int ret = rte_eth_tx_burst(dstpid, qid, &mbufs[i], 1);
          if  (ret < 1) rte_pktmbuf_free(mbufs[i]);
        }
#endif
      }
    }
  }
}

namespace dpdk {
inline size_t eth_dev_attach_slank(const char* devargs)
{
  uint8_t new_pid;
  int ret = rte_eth_dev_attach_slank(devargs, &new_pid);
  if (ret < 0) {
    std::string err = dpdk::format("dpdk::eth_dev_attach (ret=%d)", ret);
    throw dpdk::exception(err.c_str());
  }
  return new_pid;
}
} // ns dpdk

void debug(const rte_mempool* mp)
{
  while (running) {
    dpdk::mp_dump(mp);
    printf("-----------\n");
    sleep(1);
  }
}

int main(int argc, char** argv)
{
  dpdk::dpdk_boot_nopciattach(argc, argv);
  size_t pid0 = dpdk::eth_dev_attach_slank("net_pipe4,attach=net_pipe1");
  size_t pid1 = dpdk::eth_dev_attach_slank("net_pipe5,attach=net_pipe2");
  printf("pid0=%zd\n", pid0);
  printf("pid1=%zd\n", pid1);
  ports[0] = pid0;
  ports[1] = pid1;
  printf("n_ports: %u\n", rte_eth_dev_count());

  struct rte_eth_conf port_conf;
  dpdk::init_portconf(&port_conf);
  struct rte_mempool* mp = dpdk::mp_alloc("RXMBUFMPc", 1, 81920);

  const size_t n_ports = rte_eth_dev_count();
  if (n_ports != 2) throw dpdk::exception("invalid n_port");
  printf("%zd ports found \n", n_ports);
  for (size_t i=0; i<n_ports; i++) {
    dpdk::port_configure(ports[i], n_rxq, n_txq, &port_conf, mp);
  }

  running = true;
  signal(SIGINT , signal_hancler);
  signal(SIGTERM, signal_hancler);

  rte_eal_remote_launch(polling_port, nullptr, 30);
  std::thread t(debug, mp);
  rte_eal_mp_wait_lcore();
  t.join();

  for (size_t i=0; i<n_ports; i++) {
    rte_eth_dev_stop(ports[i]);
    rte_eth_dev_close(ports[i]);
  }
  rte_mempool_free(mp);
  printf("bye...\n");
}



