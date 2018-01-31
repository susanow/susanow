

#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <dpdk/wrap.h>
#include <ssn_vnf.h>
#include <ssn_common.h>
#include <ssn_vnf_port.h>
#include <vnfs/l2fwd.h>
#include <ports/dpdk_pipe.h>


int main(int argc, char** argv)
{
  ssn_subproc_init(argc, argv);
  struct rte_mempool* mp = dpdk::mp_alloc("client", 0, 81920);

  ssn_vnf_port_dpdk_pipeattach port0("port0", "net_pipe1");
  port0.set_mp(mp);
  port0.config_hw(8,8);

  ssn_vnf_port_dpdk_pipeattach port1("port1", "net_pipe2");
  port1.set_mp(mp);
  port1.config_hw(8,8);

  ssn_vnf_l2fwd vnf0("vnf13");
  vnf0.attach_port(0, &port0);
  vnf0.attach_port(1, &port1);

  vnf0.reset();
  vnf0.set_coremask(0, 0b100000000);
  vnf0.deploy();

  getchar();
  vnf0.undeploy();
  rte_mempool_free(mp);
  ssn_fin();
  printf("bye...\n");
}



