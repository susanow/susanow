

#include <stdio.h>
#include <unistd.h>
#include <susanow.h>
#include <slankdev/extra/dpdk.h>


bool running;
void l2fwd(void*)
{
  size_t nb_ports = ssn_dev_count();
  printf("start l2fwd nb_ports=%zd \n", nb_ports);
  running = true;
  while (running) {
    for (size_t pid=0; pid<nb_ports; pid++) {
      rte_mbuf* mbufs[32];
      size_t nb_recv = ssn_port_rx_burst(pid, 0, mbufs, 32);
      if (nb_recv == 0) continue;

      size_t nb_send = ssn_port_tx_burst(pid^1, 0, mbufs, nb_recv);
      if (nb_recv > nb_send) {
        ssn_mbuf_free_bulk(&mbufs[nb_send], nb_recv-nb_send);
      }
    }
    ssn_yield();
  }
}

void wait_enter(const char* msg)
{
  printf("%s [Enter]: ", msg);
  getchar();
}

class port_stats {
 public:
  rte_eth_stats cur;
  rte_eth_stats prev;

  size_t cur_rx_pps;
  size_t cur_tx_pps;
  size_t cur_rx_bps;
  size_t cur_tx_bps;

  size_t lcore_id;

  port_stats(size_t i) :
    cur_rx_pps(0), cur_tx_pps(0), cur_rx_bps(0), cur_tx_bps(0), lcore_id(i) {}
  void update()
  {
    prev = cur;
    rte_eth_stats_get(lcore_id, &cur);
    cur_rx_pps = cur.ipackets - prev.ipackets;
    cur_tx_pps = cur.opackets - prev.opackets;
    cur_rx_bps = (cur.ibytes - prev.ibytes) << 3;
    cur_tx_bps = (cur.obytes - prev.obytes) << 3;
  }
  void dump() const
  {
    printf("Port%zd    ", lcore_id);
    printf("  rx/tx[Kpps]: %zd/%zd    ", cur_rx_pps/1000, cur_tx_pps/1000);
    printf("  rx/tx[Mbps]: %zd/%zd \n", cur_rx_bps/1000000, cur_tx_bps/1000000);
  }
};

class ports_stats {
 public:
  std::vector<port_stats> ports;

  ports_stats(size_t nb)
  {
    for (size_t i=0; i<nb; i++) {
      ports.push_back(port_stats(i));
    }
  }
  void update()
  {
    size_t nb_ports = ports.size();
    for (size_t i=0; i<nb_ports; i++) {
      ports[i].update();
    }
  }
  void dump() const
  {
    size_t nb_ports = ports.size();
    for (size_t i=0; i<nb_ports; i++) {
      printf("Port%zd:", i);
      printf("rx/tx[Kpps]=%zd/%zd, rx/tx[Mbps]=%zd/%zd    ",
          ports[i].cur_rx_pps/1000,
          ports[i].cur_tx_pps/1000,
          ports[i].cur_rx_bps/1000000,
          ports[i].cur_tx_bps/1000000);
    }
    printf("\n");
  }

};

void get_dev_stat(void* arg)
{
  ports_stats* stats = reinterpret_cast<ports_stats*>(arg);
  stats->update();
  stats->dump();
}

void test(void*)
{
  while (running) {
    printf("faaa\n");
    ssn_sleep(1000);
  }
}

int main(int argc, char** argv)
{
  ssn_init(argc, argv);
  ssn_timer_sched_register(1);

  size_t nb_ports = ssn_dev_count();
  uint64_t hz = rte_get_timer_hz();
  ports_stats stats(nb_ports);
  ssn_timer* tim = ssn_timer_alloc(get_dev_stat, &stats, hz);
  ssn_timer_add(tim, 1);

  ssn_port_conf conf;
  for (size_t i=0; i<nb_ports; i++) {
    ssn_port_configure(i, &conf);
    ssn_port_dev_up(i);
    ssn_port_link_up(i);
    ssn_port_promisc_on(i);
  }

  ssn_lthread_launch(l2fwd, nullptr, 2);
  wait_enter("final");
  running = false;
  ssn_sleep(1000);

  ssn_timer_sched_unregister(1);
  ssn_fin();
}


