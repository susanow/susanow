
#pragma once
#include "func.h"


class func_wk : public func {
 public:
  func_wk(std::vector<stageio_rx*>& _rx,
          std::vector<stageio_tx*>& _tx) : func(_rx, _tx) {}
  virtual void poll_exe() override
  {
    ssn_log(SSN_LOG_INFO, "func_wk: INCLUDE DELAY\r\n");
    size_t nb_ports = ssn_dev_count();
    rte_mbuf* mbufs[32];
    while (run) {
      for (size_t p=0; p<nb_ports; p++) {
        size_t deqlen = rx[p]->rx_burst(mbufs, 32);
        for (size_t i=0; i<deqlen; i++) {
          // for (size_t j=0; j<40; j++) ; // DELAY
          int ret = tx[p^1]->tx_shot(mbufs[i]);
          if (ret < 0) rte_pktmbuf_free(mbufs[i]);
        }
      }
    }
  }
  virtual void stop() override { run = false; }
};

template <class T>
func* falloc(void* arg)
{
  stage* stg = reinterpret_cast<stage*>(arg);
  func* f = new T(stg->rx, stg->tx);
  return f;
}

class vnf_l2fwd : public vnf {
 public:
  vnic* nic0;
  vnic* nic1;
  vnf_l2fwd(vnic* n0, vnic* n1) : nic0(n0), nic1(n1)
  {
    stage* wk = new stage("wk", falloc<func_wk>);
    wk->add_nic(nic0);
    wk->add_nic(nic1);
    stages.push_back(wk);
  }
};


