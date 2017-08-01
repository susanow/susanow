
#include "nfvi.h"
#include "vty_config.h"
#include "rest_config.h"

#include <stdio.h>
#include <ssn_log.h>
#include <ssn_port.h>
#include <ssn_sys.h>
#include <ssn_common.h>

#include <stdio.h>
#include <string>

#include <slankdev/exception.h>
#include <slankdev/util.h>
#include <slankdev/extra/dpdk.h>

#include <ssn_sys.h>
#include <ssn_log.h>
#include <ssn_ring.h>
#include <ssn_port.h>
#include <ssn_port_stat.h>
#include <ssn_timer.h>
#include <ssn_native_thread.h>
#include <ssn_green_thread.h>


/*
 * Implementation
 */
void nfvi::green_thread_launch(ssn_function_t f, void* arg)
{ ssn_green_thread_launch(f, arg, green_thread_lcoreid); }

struct _vswitch_thread_arg {
  ssn_ring* ring_rx[2];
  ssn_ring* ring_tx[2];
};
static void _vswitch_thread(void* arg)
{
  _vswitch_thread_arg* sa = reinterpret_cast<_vswitch_thread_arg*>(arg);
  ssn_ring* ring_rx[2];
  ssn_ring* ring_tx[2];
  ring_rx[0] = sa->ring_rx[0];
  ring_rx[1] = sa->ring_rx[1];
  ring_tx[0] = sa->ring_tx[0];
  ring_tx[1] = sa->ring_tx[1];

  rte_mbuf* mbufs[32];
  constexpr size_t nb_ports = 2;
  while (true) {
    for (size_t p=0; p<nb_ports; p++) {
      size_t recvlen = rte_eth_rx_burst(p, 0, mbufs, 32);
      size_t enqlen  = ring_rx[p]->enq_bulk((void**)mbufs, recvlen);
      if (recvlen > enqlen)
        slankdev::rte_pktmbuf_free_bulk(&mbufs[enqlen], recvlen-enqlen);

      size_t deqlen = ring_tx[p]->deq_bulk((void**)mbufs, 32);
      size_t sendlen = rte_eth_tx_burst(p, 0, mbufs, deqlen);
      if (deqlen > sendlen)
        slankdev::rte_pktmbuf_free_bulk(&mbufs[sendlen], deqlen-sendlen);
    }
  }
}

nfvi::nfvi(int argc, char** argv)
{
  ssn_log_set_level(SSN_LOG_EMERG);
  /*
   * Main Routine
   */
  ssn_init(argc, argv);
  if (ssn_lcore_count() <= required_nb_cores)
    throw slankdev::exception("cpu tarinai");

  /* Init Green Thread Scheduler */
  ssn_green_thread_sched_register(1);

  /* Init Timer Scheduler */
  timer_sched = new ssn_timer_sched(2);
  ssn_native_thread_launch(ssn_timer_sched_poll_thread, timer_sched, 2);
  uint64_t hz = ssn_timer_get_hz();
  timer_sched->add(new ssn_timer(ssn_port_stat_update, nullptr, hz));
  timer_sched->add(new ssn_timer(ssn_ring_getstats_timer_cb, nullptr, hz));

  /* Init VTY */
  vty = new ssn_vty(0x0, vty_port);
  vty->install_command(vtymt_slank()    , vtycb_slank    , nullptr);
  vty->install_command(vtymt_show_port(), vtycb_show_port, nullptr);
  vty->install_command(vtymt_show_ring(), vtycb_show_ring, nullptr );
  // vty->install_command(vtymt_show_vnf() , vtycb_show_vnf , &vnf0  );
  ssn_green_thread_launch(ssn_vty_poll_thread, vty, 1);

  /* Init REST API Server */
  rest = new ssn_rest(0x0, rest_port);
  rest->add_route("/"       , restcb_root  , nullptr);
  rest->add_route("/stats"  , restcb_stats , nullptr);
  rest->add_route("/author" , restcb_author, nullptr);
  ssn_green_thread_launch(ssn_rest_poll_thread, rest,1);

  /* Port Configuration */
  ssn_port_conf conf;
  size_t nb_ports = ssn_dev_count();
  for (size_t i=0; i<nb_ports; i++) {
    ssn_port_configure(i, &conf);
    ssn_port_dev_up(i);
    ssn_port_promisc_on(i);
  }

  nb_ports = ssn_dev_count();
  for (size_t i=0; i<nb_ports; i++) {
    ssn_ring* r;
    std::string name;
    name = slankdev::format("rx%zd", i);
    ring_rx.push_back(new ssn_ring(name.c_str()));
    name = slankdev::format("tx%zd", i);
    ring_tx.push_back(new ssn_ring(name.c_str()));
  }

  _vswitch_thread_arg sa;
  sa.ring_rx[0] = ring_rx[0];
  sa.ring_rx[1] = ring_rx[1];
  sa.ring_tx[0] = ring_tx[0];
  sa.ring_tx[1] = ring_tx[1];
  ssn_native_thread_launch(_vswitch_thread, &sa, vswitch_thread_lcoreid);
}

nfvi::~nfvi()
{
  /*
   * Finilize
   */
  ssn_wait_all_lcore();
  ssn_green_thread_sched_unregister(1);
  delete timer_sched;
  ssn_wait_all_lcore();
  ssn_fin();
}

void nfvi::connect_vp(vnic* nic0, size_t nic1)
{
  nic0->rx = ring_rx[nic1];
  nic0->tx = ring_tx[nic1];
}

void nfvi::connect_vv(vnic* nic0, vnic* nic1)
{
  ssn_ring* ring_1to0 = new ssn_ring("1to0");
  ssn_ring* ring_0to1 = new ssn_ring("0to1");
  nic0->rx = nic1->tx = ring_1to0;
  nic1->rx = nic0->tx = ring_0to1;
}

