

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

#include "vnf.h"
#include "nfvi.h"


size_t rx_pps_sum()
{
  size_t sum = 0;
  size_t nb_ports = ssn_dev_count();
  for (size_t i=0; i<nb_ports; i++) {
    sum += ssn_port_stat_get_cur_rx_pps(i);
  }
  return sum;
}
size_t tx_pps_sum()
{
  size_t sum = 0;
  size_t nb_ports = ssn_dev_count();
  for (size_t i=0; i<nb_ports; i++) {
    sum += ssn_port_stat_get_cur_tx_pps(i);
  }
  return sum;
}

void print(void* arg)
{
  vnf* v = reinterpret_cast<vnf*>(arg);
  while (true) {
    size_t pps;
    pps = rx_pps_sum();
    printf("rx: %zd pps\n", pps);
    v->debug_dump(stdout);
    pps = tx_pps_sum();
    printf("tx: %zd pps\n", pps);
    printf("-------------\n");

    ssn_sleep(1000);
    ssn_yield();
  }
}


/* VNF IMPLEMENTATION BEGIN */
#include "l2fwd.h"
/* VNF IMPLEMENTATION END */


int main(int argc, char** argv)
{
  nfvi ssn(argc, argv);
  if (ssn_dev_count() != 2) throw slankdev::exception("nb_ports is not 2");

  vnic vnic0;
  vnic vnic1;
  vnf_l2fwd vnf1(vnic0, vnic1);

  vnic vnic2;
  vnic vnic3;
  vnf_l2fwd vnf2(vnic2, vnic3);

  ssn.connect_vp(&vnic0, 0);
  ssn.connect_vv(&vnic1, &vnic2);
  ssn.connect_vp(&vnic3, 1);

  vnf1.deploy();
  vnf2.deploy();

  ssn.green_thread_launch(print, &vnf1);
  while (true) {
    char c = getchar();
    if (c == 'q') {
      printf("quit\n");
      break;
    }
    vnf1.tuneup();
  }
}



