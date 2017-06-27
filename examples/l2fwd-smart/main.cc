

#include <stdio.h>
#include <unistd.h>
#include <susanow.h>
#include <slankdev/extra/dpdk.h>

size_t num0 = 0;
size_t num1 = 1;
size_t num2 = 2;
size_t num3 = 3;
size_t num4 = 4;

rte_ring* pre_tx[2];
rte_ring* post_rx[2];

bool running = true;
void rx_q(void* arg)
{
  size_t qid = *reinterpret_cast<size_t*>(arg);
  size_t nb_ports = ssn_dev_count();
  printf("start rx-thread nb_ports=%zd \n", nb_ports);
  while (running) {
    for (size_t pid=0; pid<nb_ports; pid++) {
      rte_mbuf* mbufs[32];
      size_t nb_recv = ssn_port_rx_burst(pid, qid, mbufs, 32);
      if (nb_recv == 0) continue;

      size_t nb_enq = rte_ring_enqueue_bulk(post_rx[pid], (void**)mbufs, nb_recv, nullptr);
      if (nb_enq < nb_recv)
        ssn_mbuf_free_bulk(&mbufs[nb_enq], nb_recv-nb_enq);
    }
  }
}
void rx(void*)
{
  size_t nb_ports = ssn_dev_count();
  printf("start rx-thread nb_ports=%zd \n", nb_ports);
  while (running) {
    for (size_t pid=0; pid<nb_ports; pid++) {
      rte_mbuf* mbufs[32];
      size_t nb_recv = ssn_port_rx_burst(pid, 0, mbufs, 32);
      if (nb_recv == 0) continue;

      size_t nb_enq = rte_ring_enqueue_bulk(post_rx[pid], (void**)mbufs, nb_recv, nullptr);
      if (nb_enq < nb_recv)
        ssn_mbuf_free_bulk(&mbufs[nb_enq], nb_recv-nb_enq);
    }
  }
}
void wk(void*)
{
  static size_t count = 0;
  count ++;
  size_t nb_ports = ssn_dev_count();
  printf("start wk-thread nb_ports=%zd nb_wks=%zd \n", nb_ports, count);
  while (running) {
    for (size_t pid=0; pid<nb_ports; pid++) {
#if 1
      rte_mbuf* mbufs[32];
      size_t nb_deq = rte_ring_dequeue_bulk(post_rx[pid], (void**)mbufs, 32, nullptr);
      if (nb_deq == 0) continue;
      rte_delay_us_block(10);
      size_t nb_enq = rte_ring_enqueue_bulk(pre_tx[pid^1], (void**)mbufs, nb_deq, nullptr);
      if (nb_enq < nb_deq)
        ssn_mbuf_free_bulk(&mbufs[nb_enq], nb_deq-nb_enq);
#else
      rte_mbuf* msg;
      int ret = rte_ring_dequeue(post_rx[pid], (void**)&msg);
      if (ret < 0) continue;
      rte_delay_us_block(1);
      ret = rte_ring_enqueue(pre_tx[pid^1], msg);
      if (ret < 0) rte_pktmbuf_free(msg);
#endif
    }
  }
}
void tx(void*)
{
  size_t nb_ports = ssn_dev_count();
  printf("start tx-thread nb_ports=%zd \n", nb_ports);
  while (running) {
    for (size_t pid=0; pid<nb_ports; pid++) {
      rte_mbuf* mbufs[32];
      size_t nb_deq = rte_ring_dequeue_bulk(pre_tx[pid], (void**)mbufs, 32, nullptr);
      size_t nb_send = ssn_port_tx_burst(pid, 0, mbufs, nb_deq);
      if (nb_deq > nb_send) {
        ssn_mbuf_free_bulk(&mbufs[nb_send], nb_deq-nb_send);
      }
    }
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

int main(int argc, char** argv)
{
  ssn_log_set_level(SSN_LOG_DEBUG);
  ssn_init(argc, argv);
  ssn_timer_sched_register(1);

  size_t nb_ports = ssn_dev_count();
  uint64_t hz = rte_get_timer_hz();
  ports_stats stats(nb_ports);
  ssn_timer* tim = ssn_timer_alloc(get_dev_stat, &stats, hz);
  ssn_timer_add(tim, 1);

  ssn_port_conf conf;
  conf.nb_rxq = 2;
  for (size_t i=0; i<nb_ports; i++) {
    ssn_port_configure(i, &conf);
    ssn_port_dev_up(i);
    ssn_port_link_up(i);
    ssn_port_promisc_on(i);
  }

  post_rx[0] = slankdev::ring_alloc("post_rx[0]", 1024);
  post_rx[1] = slankdev::ring_alloc("post_rx[1]", 1024);
  pre_tx[0]  = slankdev::ring_alloc("pre_tx[0] ", 1024);
  pre_tx[1]  = slankdev::ring_alloc("pre_tx[1] ", 1024);

  ssn_native_thread_launch(rx_q, &num0, 2);
  ssn_native_thread_launch(rx_q, &num1, 7);
  ssn_native_thread_launch(tx, nullptr, 3);
  wait_enter("next");
  ssn_native_thread_launch(wk, nullptr, 4);
  wait_enter("next");
  ssn_native_thread_launch(wk, nullptr, 5);
  wait_enter("next");
  ssn_native_thread_launch(wk, nullptr, 6);

  wait_enter("Type Enter to Exit L2fwd");
  running = false;

  ssn_timer_sched_unregister(1);
  ssn_fin();

  rte_ring_free(post_rx[0]);
  rte_ring_free(post_rx[1]);
  rte_ring_free(pre_tx[0] );
  rte_ring_free(pre_tx[1] );

}


