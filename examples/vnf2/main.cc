

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


/* VNF IMPLEMENTATION BEGIN */
#include "l2fwd.h"
/* VNF IMPLEMENTATION END */


int main(int argc, char** argv)
{
  nfvi ssn(argc, argv);
  if (ssn_dev_count() != 2) throw slankdev::exception("nb_ports is not 2");

  vnic* vnic0 = new vnic;
  vnic* vnic1 = new vnic;
  ssn.connect(vnic0, 0);
  ssn.connect(vnic1, 1);
  vnf_l2fwd* vnf1 = new vnf_l2fwd(vnic0, vnic1);

  vnf1->deploy();

  // ssn.green_thread_launch(print, vnf1);
  while (true) {
    char c = getchar();
    if (c == 'q') {
      printf("quit\n");
      break;
    }
    vnf1->tuneup();
  }
}



