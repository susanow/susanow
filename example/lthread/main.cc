
#include <stdio.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>

#include <rte_config.h>
#include <rte_version.h>
#include <rte_eal.h>
#include <rte_ethdev.h>
#include <rte_ether.h>
#include <rte_cycles.h>
#include <rte_lcore.h>
#include <rte_mbuf.h>
#include <rte_hexdump.h>
#include <rte_ip.h>
#include <rte_ip_frag.h>
#include <lthread_api.h>
#include "l3fwd.h"

volatile bool force_quit;


RTE_DEFINE_PER_LCORE(struct lcore_conf *, lcore_conf);

class test {
    int a;
};


static void lthread_start(void*)
{
    int lcore_id = rte_lcore_id ();
	while (1) {
		printf("strat %d\n", lcore_id);
		/* sleep(1); */
      lthread_yield ();
	}
    lthread_exit (NULL);
}

static void lthread_tap_manager(void*)
{
    int lcore_id = rte_lcore_id ();
	while (1) {
		printf("tap mana %d \n", lcore_id);
		/* sleep(1); */
      lthread_yield ();
	}
    lthread_exit (NULL);
}

struct lcore_conf lcore_conf[RTE_MAX_LCORE];

void lthread_schedular()
{
  int lcore_id = rte_lcore_id ();
  struct lthread *lt[2];

  RTE_PER_LCORE (lcore_conf) = &lcore_conf[lcore_id];

  lthread_create (&lt[0], -1, lthread_tap_manager, NULL);
  lthread_create (&lt[1], -1, lthread_start, NULL);

  lthread_run ();
}


int main(int argc, char** argv)
{
    rte_eal_init(argc, argv);

    lthread_schedular();
}
