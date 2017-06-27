

#include <stdio.h>
#include <unistd.h>
#include <susanow.h>
#include <slankdev/extra/dpdk.h>
#include <ssn_port_stat.h>


rte_ring* pre_tx[2];
rte_ring* post_rx[2];

bool running = true;
void rx(void*)
{
  printf("start rx-thread \n");
  size_t nb_ports = ssn_dev_count();
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
  static size_t count = 0; count ++;
  printf("start wk-thread nb_wks=%zd \n", count);
  size_t nb_ports = ssn_dev_count();
  while (running) {
    for (size_t pid=0; pid<nb_ports; pid++) {
      rte_mbuf* msg;
      int ret = rte_ring_dequeue(post_rx[pid], (void**)&msg);
      if (ret < 0) continue;
      rte_delay_us_block(1);
      ret = rte_ring_enqueue(pre_tx[pid^1], msg);
      if (ret < 0) rte_pktmbuf_free(msg);
    }
  }
}
void tx(void*)
{
  size_t nb_ports = ssn_dev_count();
  printf("start tx-thread \n");
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


#if 0
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
#endif

void get_dev_stat(void* arg)
{
  // ports_stats* stats = reinterpret_cast<ports_stats*>(arg);
  // stats->update();
  // stats->dump();
}

int main(int argc, char** argv)
{
  ssn_init(argc, argv);
  ssn_port_stat_init(); //ERASE
  ssn_timer_sched_register(1);

  // ports_stats stats(nb_ports);
  uint64_t hz = rte_get_timer_hz();
  ssn_timer* tim = ssn_timer_alloc(ssn_port_stat_update, nullptr, hz);
  ssn_timer_add(tim, 1);

  size_t nb_ports = ssn_dev_count();
  ssn_port_conf conf;
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

  ssn_native_thread_launch(rx, nullptr, 2);
  ssn_native_thread_launch(tx, nullptr, 3); wait_enter("next");
  ssn_native_thread_launch(wk, nullptr, 4); wait_enter("next");
  ssn_native_thread_launch(wk, nullptr, 5); wait_enter("next");
  ssn_native_thread_launch(wk, nullptr, 6);

  wait_enter("Type Enter to Exit L2fwd");
  running = false;

  ssn_timer_sched_unregister(1);
  ssn_port_stat_fin(); //ERASE
  ssn_fin();

  rte_ring_free(post_rx[0]);
  rte_ring_free(post_rx[1]);
  rte_ring_free(pre_tx[0] );
  rte_ring_free(pre_tx[1] );
}


