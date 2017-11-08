

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
  ssn_log(SSN_LOG_INFO, "start rx-thread \n");
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
  ssn_log(SSN_LOG_INFO, "finish rx-thread \n");
}
void wk(void*)
{
  static size_t count = 0; count ++;
  ssn_log(SSN_LOG_INFO, "start wk-thread nb_wks=%zd \n", count);
  size_t nb_ports = ssn_dev_count();
  while (running) {
    for (size_t pid=0; pid<nb_ports; pid++) {
      rte_mbuf* msg;
      int ret = rte_ring_dequeue(post_rx[pid], (void**)&msg);
      if (ret < 0) continue;

      // rte_delay_us_block(1);
      // for (size_t i=0; i<10; i++) ;

      ret = rte_ring_enqueue(pre_tx[pid^1], msg);
      if (ret < 0) rte_pktmbuf_free(msg);
    }
  }
  ssn_log(SSN_LOG_INFO, "finish wk-thread nb_wks=%zd \n", count);
}
void tx(void*)
{
  size_t nb_ports = ssn_dev_count();
  ssn_log(SSN_LOG_INFO, "start tx-thread \n");
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
  ssn_log(SSN_LOG_INFO, "finish tx-thread\n");
}
#if 0
void print_stats(void*)
{
  while (running) {
    size_t nb_ports = rte_eth_dev_count();
    printf("  %5s %20s %20s %20s %20s\n",
        "idx", "rx[bps]", "rx[pps]", "tx[bps]", "tx[pps]");
    printf("  ----------------------------------------");
    printf("-----------------------------------------------------\n");
    for (size_t i=0; i<nb_ports; i++) {
      size_t rx_bps = ssn_port_stat_get_cur_rx_pps(i);
      size_t tx_bps = ssn_port_stat_get_cur_tx_pps(i);
      size_t rx_pps = ssn_port_stat_get_cur_rx_bps(i);
      size_t tx_pps = ssn_port_stat_get_cur_tx_bps(i);
      printf("  %5zd %20zd %20zd %20zd %20zd\n", i,
          rx_bps, rx_pps, tx_bps, tx_pps);
    }
    printf("\n\n");
    ssn_sleep(1000);
  }
}
#endif

bool waiter_running = true;
void waiter(void*)
{
  size_t lcore_id = ssn_lcore_id();
  while (waiter_running) {
    size_t nb_lcores = ssn_lcore_count();
    for (size_t i=0; i<nb_lcores; i++) {
      if (ssn_lcore_joinable(i)) {
        ssn_lcore_join(i);
      }
    }
    if (is_green_thread(lcore_id)) ssn_yield();
  }
}

ssize_t found_free_lcore()
{
  size_t nb_lcores = ssn_lcore_count();
  for (size_t i=1; i<nb_lcores; i++) {
    ssn_lcore_state s = ssn_get_lcore_state(i);
    if (s == SSN_LS_WAIT) return i;
  }
  return -1;
}
void optimize()
{
  printf("==Check=Performance=========\n");
  size_t nb_ports = rte_eth_dev_count();
  size_t rx_bps_sum = 0;
  size_t tx_bps_sum = 0;
  for (size_t i=0; i<nb_ports; i++) {
    size_t rx_bps = ssn_port_stat_get_cur_rx_bps(i);
    size_t tx_bps = ssn_port_stat_get_cur_tx_bps(i);
    rx_bps_sum += rx_bps;
    tx_bps_sum += tx_bps;
  }
  printf("rx: %5zd [Mbps]\n", rx_bps_sum/1000000);
  printf("tx: %5zd [Mbps]\n", tx_bps_sum/1000000);
  double reduction_rate = (100-(double)tx_bps_sum/rx_bps_sum*100);
  printf("reduction rate: %.2lf%% \n", reduction_rate);
  if (reduction_rate > 10) {
    ssize_t ret = found_free_lcore();
    if (ret > 0) {
      printf("Mux wk thread to lcore%zd\n", ret);
      ssn_native_thread_launch(wk, nullptr, ret);
    } else {
      printf("No waiting lcores. no operation\n");
    }
  } else {
    printf("No Performance Reduction\n");
    printf("No operation\n");
  }
  printf("===========================\n");
}


int main(int argc, char** argv)
{
  ssn_log_set_level(SSN_LOG_ERR);
  ssn_init(argc, argv);
  ssn_timer_sched_register(1);
  ssn_green_thread_sched_register(2);

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

  // ssn_green_thread_launch(print_stats, nullptr, 2);
  // ssn_green_thread_launch(ssn_vty_thread, nullptr, 2);
  ssn_green_thread_launch(waiter, nullptr, 2);

  ssn_native_thread_launch(rx, nullptr, 3);
  ssn_native_thread_launch(tx, nullptr, 4);
  ssn_native_thread_launch(wk, nullptr, 5);
  ssn_sleep(1000);
  printf("---START-----------\n");

  while (true) {
    char c = getchar();
    if (c == 'q') break;
    optimize();
  }

  printf("next finish \n");
  getchar();

  printf("---FINISH----------\n");
  running = false;
  waiter_running = false;

  ssn_timer_del(tim, 1);
  ssn_timer_free(tim);
  ssn_timer_sched_unregister(1);
  ssn_green_thread_sched_unregister(2);

  ssn_wait_all_lcore();
  rte_ring_free(post_rx[0]);
  rte_ring_free(post_rx[1]);
  rte_ring_free(pre_tx[0] );
  rte_ring_free(pre_tx[1] );
  ssn_fin();
}


