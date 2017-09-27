

#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <slankdev/string.h>
#include <slankdev/exception.h>

#include <ssn_common.h>
#include <ssn_log.h>
#include <ssn_vnf.h>

void INIT(int argc, char** argv)
{
  ssn_init(argc, argv);
  size_t n_ports = ssn_dev_count();
  if (n_ports != 2) {
    std::string err = slankdev::format("n_ports is not 2 (current %zd)",
        ssn_dev_count());
    throw slankdev::exception(err.c_str());
  }
}

class vnf_test : public ssn_vnf {
  bool l2fwd_running;
  virtual bool is_running() const override { return l2fwd_running; }
  virtual void undeploy_imple() override { l2fwd_running=false; }
  virtual void deploy_imple(void*) override
  {
    l2fwd_running = true;
    size_t aid = get_aid();
    ssn_log(SSN_LOG_INFO, "start new thread %s, access_id=%zd\n", __func__, aid);

    size_t nb_ports = n_ports();
    while (l2fwd_running) {
      for (size_t pid=0; pid<nb_ports; pid++) {
        rte_mbuf* mbufs[32];
        size_t nb_recv = rx_burst(pid, aid, mbufs, 32);
        if (nb_recv == 0) continue;

        for (size_t i=0; i<nb_recv; i++) {

          /* Delay Block begin */
          size_t n=10;
          for (size_t j=0; j<100; j++) n++;

          size_t nb_send = tx_burst(pid^1, aid, &mbufs[i], 1);
          if (nb_send != 1)
            rte_pktmbuf_free(mbufs[i]);
        }
      }
    } /* while */

    ssn_log(SSN_LOG_INFO, "finish thread %s \n", __func__);
  }
 public:
  vnf_test(size_t np) : ssn_vnf(np), l2fwd_running(false) {}
};

char waitmsg(const char* msg)
{
  printf(msg);
  return getchar();
}

void VNF_DUMP(ssn_vnf* vnf)
{
  printf("\n");
  printf("vnfptr: %p \r\n", vnf);
  vnf->debug_dump(stdout);
  printf("\n");
}

int main(int argc, char** argv)
{
  INIT(argc, argv);

  ssn_vnf_port* port0 = new ssn_vnf_port(0, 4, 4); // dpdk0
  ssn_vnf_port* port1 = new ssn_vnf_port(1, 4, 4); // dpdk1
  printf("\n");
  port0->debug_dump(stdout); printf("\n");
  port1->debug_dump(stdout); printf("\n");

  const size_t n_ports = 2;
  vnf_test* vnf0 = new vnf_test(n_ports);
  vnf0->attach_port(0, port0); // attach dpdk0 to vnf0
  vnf0->attach_port(1, port1); // attach dpdk1 to vnf0
  VNF_DUMP(vnf0);

  /* run with 1 cores */
  waitmsg("press [enter] to deploy with 1 lcores...\n");
  vnf0->undeploy();
  vnf0->deploy(0x04);
  VNF_DUMP(vnf0);

  /* run with 2 cores */
  waitmsg("press [enter] to deploy with 2 lcores...\n");
  vnf0->undeploy();
  vnf0->deploy(0x0c);
  VNF_DUMP(vnf0);

  /* run with 4 cores */
  waitmsg("press [enter] to deploy with 4 lcores...\n");
  vnf0->undeploy();
  vnf0->deploy(0x3c);
  VNF_DUMP(vnf0);

  /* undeploy */
  waitmsg("press [enter] to undeploy ");
  vnf0->undeploy();
  VNF_DUMP(vnf0);

fin:
  delete vnf0;
  delete port0;
  delete port1;
  ssn_fin();
}


