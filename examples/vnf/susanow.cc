

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

#include "susanow.h"
#include "vty_config.h"
#include "rest_config.h"




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
  vty = new ssn_vty(0x0, vty_port);
  vty->install_command(vtymt_slank()    , vtycb_slank    , nullptr);
  vty->install_command(vtymt_show_port(), vtycb_show_port, nullptr);
  vty->install_command(vtymt_show_ring(), vtycb_show_ring, &rings );
  // vty->install_command(vtymt_show_vnf() , vtycb_show_vnf , &vnf0  );
  ssn_green_thread_launch(ssn_vty_poll_thread, vty, 1);

  /*
   * Init REST API Server
   */
  rest = new ssn_rest(0x0, rest_port);
  rest->add_route("/"       , restcb_root  , nullptr);
  rest->add_route("/stats"  , restcb_stats , nullptr);
  rest->add_route("/author" , restcb_author, nullptr);
  ssn_green_thread_launch(ssn_rest_poll_thread, rest,1);

  /*
   * Port Configuration
   */
  ssn_port_conf conf;
  size_t nb_ports = ssn_dev_count();
  for (size_t i=0; i<nb_ports; i++) {
    ssn_port_configure(i, &conf);
    ssn_port_dev_up(i);
    ssn_port_promisc_on(i);
  }

  /*
   * Init VNF
   */
  ssn_ring* prewk[2];
  ssn_ring* poswk[2];
  prewk[0] = ring_alloc("prewk0");
  prewk[1] = ring_alloc("prewk1");
  poswk[0] = ring_alloc("poswk0");
  poswk[1] = ring_alloc("poswk1");
  vnf1 = new vnf_l2fwd(prewk, poswk);
  vnf1->deploy();

}

void ssn::launch_green_thread(ssn_function_t f, void* arg)
{ ssn_green_thread_launch(f, arg, 1); }

void ssn::debug_dump(FILE* fp)
{
  vnf1->debug_dump(fp);
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


