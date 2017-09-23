
#include <stdio.h>
#include <stddef.h>
#include <stdint.h>
#include <vector>
#include <utility>
#include <slankdev/exception.h>
#include "ssn_vnf_port.h"

/*-------------------User-Code-Below-------------------------*/

ssn_vnf_port p;
size_t num[] = {0,1,2,3,4,5,6,7,8};

void l2fwd(void* acc_id_)
{
  size_t aid = *((size_t*)acc_id_);
  printf("access id %zd \n", aid);

  rte_mbuf mbufs[32];
  p.rx_burst(aid, mbufs, 32);
  p.rx_burst(aid, mbufs, 32);
  p.rx_burst(aid, mbufs, 32);
  p.rx_burst(aid, mbufs, 32);
  p.rx_burst(aid, mbufs, 32);
  p.rx_burst(aid, mbufs, 32);
  p.rx_burst(aid, mbufs, 32);
  p.rx_burst(aid, mbufs, 32);
  p.rx_burst(aid, mbufs, 32);
  printf("\n");
  p.tx_burst(aid, mbufs, 2);
  p.tx_burst(aid, mbufs, 2);
  p.tx_burst(aid, mbufs, 2);
  p.tx_burst(aid, mbufs, 2);
  p.tx_burst(aid, mbufs, 2);

  printf("\n");
  p.rx_burst(aid, mbufs, 32);
  p.tx_burst(aid, mbufs, 2);
  p.tx_burst(aid, mbufs, 2);
  p.tx_burst(aid, mbufs, 2);
  p.rx_burst(aid, mbufs, 32);
}

int main(int argc, char** argv)
{
  size_t n_que = 4;
  size_t n_acc = 2;
  p.configuration(0,n_que,n_acc,n_que,n_acc);

  printf("===========================================\n");
  printf("===========================================\n");
  printf("\n\n");

  l2fwd(&num[0]); printf("\n\n");
  l2fwd(&num[1]); printf("\n\n");
  // l2fwd(&num[2]); printf("\n\n");
  // l2fwd(&num[3]); printf("\n\n");
}


