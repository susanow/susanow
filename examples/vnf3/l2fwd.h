

#pragma once

class func_rx : public func {
  bool run;
 public:
  ssn_ring* rx[2];
  ssn_ring* tx[2];
  virtual void poll_exe() override
  {
    rte_mbuf* mbufs[32];
    size_t nb_ports = ssn_dev_count();
    while (run) {
      for (size_t p=0; p<nb_ports; p++) {
        size_t deqlen = rx[p]->deq_burst((void**)mbufs, 32);

        int n=0;
        for (size_t J=0; J<400; J++) n++; //DELAY

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
  ssn_ring* rx[2];
  ssn_ring* tx[2];
  stage_rx(const char* n, ssn_ring* rx0, ssn_ring* rx1, ssn_ring* wk0, ssn_ring* wk1) : stage(n)
  {
    rx[0] = rx0;
    rx[1] = rx1;
    tx[0] = wk0;
    tx[1] = wk1;
  }
  virtual func* falloc() override
  {
    func_rx* f = new func_rx;
    f->rx[0] = rx[0];
    f->rx[1] = rx[1];
    f->tx[0] = tx[0];
    f->tx[1] = tx[1];
    return f;
  }
  virtual size_t throughput_pps() const override
  {
    size_t sum = 0;
    for (size_t i=0; i<2; i++) {
      sum += rx[i]->opps;
    }
    return sum;
  }
  virtual double reduction_rate() const override
  {
    size_t pps = throughput_pps();
    size_t pps_p = 0;
    for (size_t i=0; i<2; i++) {
      pps_p += rx[i]->ipps;
    }
    return double(pps)/double(pps_p);
  }
};

class func_tx : public func {
  bool run;
 public:
  ssn_ring* rx[2];
  ssn_ring* tx[2];
  virtual void poll_exe() override
  {
    rte_mbuf* mbufs[32];
    size_t nb_ports = ssn_dev_count();
    while (run) {
      for (size_t p=0; p<nb_ports; p++) {
        size_t deqlen = rx[p]->deq_burst((void**)mbufs, 32);

        int n=0;
        for (size_t J=0; J<400; J++) n++; //DELAY

        for (size_t i=0; i<deqlen; i++) {
          int ret = tx[p]->enq(mbufs[i]);
          if (ret < 0) rte_pktmbuf_free(mbufs[i]);
        }
      }
    }
  }
  virtual void stop() override { run = false; }
};

class stage_tx : public stage {
 public:
  ssn_ring* rx[2];
  ssn_ring* tx[2];
  stage_tx(const char* n, ssn_ring* wk0, ssn_ring* wk1, ssn_ring* tx0, ssn_ring* tx1) : stage(n)
  {
    rx[0] = wk0;
    rx[1] = wk1;
    tx[0] = tx0;
    tx[1] = tx1;
  }
  virtual func* falloc() override
  {
    func_tx* f = new func_tx;
    f->rx[0] = rx[0];
    f->rx[1] = rx[1];
    f->tx[0] = tx[0];
    f->tx[1] = tx[1];
    return f;
  }
  virtual size_t throughput_pps() const override
  {
    size_t sum = 0;
    for (size_t i=0; i<2; i++) {
      sum += rx[i]->opps;
    }
    return sum;
  }
  virtual double reduction_rate() const override
  {
    size_t pps = throughput_pps();
    size_t pps_p = 0;
    for (size_t i=0; i<2; i++) {
      pps_p += rx[i]->ipps;
    }
    return double(pps)/double(pps_p);
  }
};

class vnf_l2fwd : public vnf {
 public:
  ssn_ring* wk[2];

  vnf_l2fwd(vnic& n0, vnic& n1)
  {
    wk[0] = new ssn_ring("0->1");
    wk[1] = new ssn_ring("1->0");

    stage* rx = new stage_rx("rx", n0.rx, n1.rx, wk[0], wk[1]);
    stages.push_back(rx);

    stage* tx = new stage_tx("rx", wk[0], wk[1], n0.tx, n1.tx);
    stages.push_back(tx);
  }
  ~vnf_l2fwd()
  {
    delete wk[0];
    delete wk[1];
  }
};


