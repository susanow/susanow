

#pragma once

class func_rx : public func {
  bool run;
 public:
  ssn_ring* rx[2];
  ssn_ring* tx[2];
  virtual void poll_exe() override
  {
    ssn_log(SSN_LOG_INFO, "func_rx: INCLUDE DELAY\r\n");
    rte_mbuf* mbufs[32];
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
  ssn_ring** rx[2];
  ssn_ring** tx[2];
  stage_rx(const char* n, vnic& n0, vnic& n1) : stage(n)
  {
    rx[0] = &n0.rx;
    rx[1] = &n1.rx;
    tx[0] = &n0.tx;
    tx[1] = &n1.tx;
  }
  virtual func* falloc() override
  {
    func_rx* f = new func_rx;
    f->rx[0] = *rx[0];
    f->rx[1] = *rx[1];
    f->tx[0] = *tx[0];
    f->tx[1] = *tx[1];
    return f;
  }
  virtual size_t throughput_pps() const override { return 0; }
};

class vnf_l2fwd : public vnf {
 public:
  ssn_ring* rx_[2];
  ssn_ring* tx_[2];

  vnf_l2fwd(vnic& n0, vnic& n1)
  {
    stage* rx = new stage_rx("rx", n0, n1);
    stages.push_back(rx);
  }
};


