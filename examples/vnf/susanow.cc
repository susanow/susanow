
#include "susanow.h"
#include "vty_config.h"
#include "rest_config.h"


ssn_ring* prewk[2];
ssn_ring* poswk[2];
bool run = true;
void rx(void*)
{
  size_t nb_ports = ssn_dev_count();
  while (run) {
    for (size_t p=0; p<nb_ports; p++) {
      rte_mbuf* mbufs[32];
      size_t recvlen = ssn_port_rx_burst(p, 0, mbufs, 32);
      if (recvlen == 0) continue;
      size_t enqlen = prewk[p]->enq_bulk((void**)mbufs, recvlen);
      if (recvlen > enqlen) {
        slankdev::rte_pktmbuf_free_bulk(&mbufs[enqlen], recvlen-enqlen);
      }
    }
  }
}

void tx(void*)
{
  size_t nb_ports = ssn_dev_count();
  while (run) {
    for (size_t p=0; p<nb_ports; p++) {
      rte_mbuf* mbufs[32];
      size_t deqlen = poswk[p]->deq_bulk((void**)mbufs, 32);
      if (deqlen == 0) continue;
      size_t sendlen = ssn_port_tx_burst(p, 0, mbufs, deqlen);
      if (deqlen > sendlen) {
        slankdev::rte_pktmbuf_free_bulk(&mbufs[sendlen], deqlen-sendlen);
      }
    }
  }
}

void wk_shot(void*)
{
  size_t nb_ports = ssn_dev_count();
  rte_mbuf* mbufs[32];
  while (run) {
    for (size_t p=0; p<nb_ports; p++) {
      size_t deqlen = prewk[p]->deq_bulk((void**)mbufs, 32);
      for (size_t i=0; i<deqlen; i++) {
        int ret = poswk[p^1]->enq(mbufs[i]);
        if (ret < 0) rte_pktmbuf_free(mbufs[i]);
      }
    }
  }
}


ssn::ssn(int argc, char** argv)
{
  ssn_init(argc, argv);
  if (ssn_lcore_count() <= required_nb_cores)
    throw slankdev::exception("cpu tarinai");

  /*
   * Init Green Thread Scheduler
   */
  ssn_green_thread_sched_register(1);

  /*
   * Init Timer Scheduler
   */
  timer_sched = new ssn_timer_sched(2);
  ssn_native_thread_launch(ssn_timer_sched_poll_thread, timer_sched, 2);
  uint64_t hz = ssn_timer_get_hz();
  timer_sched->add(new ssn_timer(ssn_port_stat_update, nullptr, hz));

  /*
   * Init VTY
   */
#if 0
  vty = new ssn_vty(0x0, vty_port);
  vty->install_command(vtymt_slank()    , vtycb_slank    , nullptr);
  vty->install_command(vtymt_show_vnf() , vtycb_show_vnf , &vnf0  );
  vty->install_command(vtymt_show_port(), vtycb_show_port, nullptr);
  vty->install_command(vtymt_show_ring(), vtycb_show_ring, &rings );
  ssn_green_thread_launch(ssn_vty_poll_thread, vty, 1);
#endif

  /*
   * Init REST API Server
   */
#if 0
  rest = new ssn_rest(0x0, rest_port);
  rest->add_route("/"       , restcb_root  , nullptr);
  rest->add_route("/stats"  , restcb_stats , nullptr);
  rest->add_route("/author" , restcb_author, nullptr);
  ssn_green_thread_launch(ssn_rest_poll_thread, rest,1);
#endif

  /*
   * Port Configuration
   */
  ssn_port_conf conf;
  size_t nb_ports = ssn_dev_count();
  for (size_t i=0; i<nb_ports; i++) {
    ssn_port_configure(i, &conf);
    ssn_port_dev_up(i);
    ssn_port_link_up(i);
    ssn_port_promisc_on(i);
  }

  /*
   * Init VNF
   */
  vnf0 = new vnf("vnf0");
  prewk[0] = ring_alloc("prewk0");
  prewk[1] = ring_alloc("prewk1");
  poswk[0] = ring_alloc("poswk0");
  poswk[1] = ring_alloc("poswk1");
  vnf0->pl.push_back(pl_stage(rx, nullptr));
  vnf0->pl.push_back(pl_stage(wk_shot, nullptr));
  vnf0->pl.push_back(pl_stage(tx, nullptr));
  vnf0->deploy();
}

void ssn::debug_dump(FILE* fp)
{
  vnf0->debug_dump(stdout);
}

ssn::~ssn()
{
  ssn_green_thread_sched_unregister(1);
  delete timer_sched;
  ssn_wait_all_lcore();
  ssn_fin();

  size_t nb_rings = rings.size();
  for (size_t i=0; i<nb_rings; i++) {
    delete rings[i];
  }
}

ssn_ring* ssn::ring_alloc(const char* name)
{
  ssn_ring* sr = new ssn_ring(name);
  uint64_t hz = ssn_timer_get_hz();
  add_timer(new ssn_timer(ssn_ring_getstats_timer_cb, sr, hz));
  rings.push_back(sr);
  return sr;
}


