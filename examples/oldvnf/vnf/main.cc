

#include <stdio.h>
#include <ssn_log.h>
#include <ssn_port.h>
#include <ssn_cpu.h>
#include <ssn_common.h>

#include <stdio.h>
#include <string>

#include <slankdev/exception.h>
#include <slankdev/util.h>
#include <dpdk/dpdk.h>

#include <ssn_cpu.h>
#include <ssn_log.h>
#include <ssn_ring.h>
#include <ssn_port.h>
#include <ssn_port_stat.h>
#include <ssn_timer.h>
#include <ssn_native_thread.h>
#include <ssn_green_thread.h>

#include "vnf.h"
#include "vty_config.h"
#include "rest_config.h"

constexpr uint16_t vty_port = 9999;
constexpr uint16_t rest_port = 8888;
constexpr size_t   required_nb_cores = 4;
constexpr size_t   green_thread_lcoreid = 1;
constexpr size_t   timer_lcoreid = 2;

void print(void* arg)
{
  vnf* v = reinterpret_cast<vnf*>(arg);
  while (true) {
    v->debug_dump(stdout);
    printf("-------------\n");
    ssn_sleep(1000);
    ssn_yield();
  }
}

void init(ssn_timer_sched* timer_sched, ssn_vty* vty, ssn_rest* rest, int argc, char** argv);
void fini(ssn_timer_sched* timer_sched, ssn_vty* vty, ssn_rest* rest);

/*
 * VNF IMPLEMENTATION BEGIN
 */
#include "l2fwd.h"
/*
 * VNF IMPLEMENTATION END
 */


int main(int argc, char** argv)
{
  ssn_timer_sched* timer_sched;
  ssn_vty* vty;
  ssn_rest* rest;

  init(timer_sched, vty, rest, argc, argv);

  /* Init VNF */
  vnf_l2fwd* vnf1 = new vnf_l2fwd;
  vnf1->deploy();
  ssn_green_thread_launch(print, vnf1, green_thread_lcoreid);

  /* Loop */
  while (true) {
    char c = getchar();
    if (c == 'q') {
      printf("quit\n");
      break;
    }
    vnf1->tuneup();
  }

  fini(timer_sched, vty, rest);
}

void init(ssn_timer_sched* timer_sched, ssn_vty* vty, ssn_rest* rest, int argc, char** argv)
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
}

void fini(ssn_timer_sched* timer_sched, ssn_vty* vty, ssn_rest* rest)
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


