
#pragma once
#include "stage.h"


class func_rx : public func {
  bool run;
 public:
  ssn_rings rx_;
  ssn_rings tx_;
  virtual void poll_exe() override
  {
    ssn_log(SSN_LOG_INFO, "func_rx: INCLUDE DELAY\r\n");

    rte_mbuf* mbufs[32];
    ssn_ring* rx[2];
    ssn_ring* tx[2];
    rx[0] = rx_[0];
    rx[1] = rx_[1];
    tx[0] = tx_[0];
    tx[1] = tx_[1];

    size_t nb_ports = ssn_dev_count();
    while (run) {
      for (size_t p=0; p<nb_ports; p++) {
        size_t deqlen = rx[p]->deq_bulk((void**)mbufs, 32);
        for (size_t i=0; i<deqlen; i++) {
          int ret = tx[p^1]->enq(mbufs[i]);
          if (ret < 0) rte_pktmbuf_free(mbufs[i]);
        }
      }
    }
  }
  virtual void stop() override { run = false; }
};

class stage_rx : public stage {
 public:
  vnic& nic0;
  vnic& nic1;
  stage_rx(const char* n, vnic& n0, vnic& n1) : stage(n),nic0(n0),nic1(n1) {}
  virtual func* falloc() override
  {
    func_rx* f = new func_rx;
    f->rx_.push_back(nic0.rx);
    f->rx_.push_back(nic1.rx);
    f->tx_.push_back(nic0.tx);
    f->tx_.push_back(nic1.tx);
    return f;
  }
  virtual size_t throughput_pps() const override { return 0; }
};

class vnf_l2fwd : public vnf {
 public:
  vnic& nic0;
  vnic& nic1;
  vnf_l2fwd(vnic& n0, vnic& n1) : nic0(n0), nic1(n1)
  {
    stage* rx = new stage_rx("rx", nic0, nic1);
    stages.push_back(rx);
  }
};


