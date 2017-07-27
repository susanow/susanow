
#pragma once

#if 0
class stage_rx : public stage {
 public:
  std::vector<stageio_rx_port> rx;
  std::vector<stageio_tx_ring> tx;

  stage_rx(const char* n) : stage(n) { }
  virtual func* allocate() override
  {
    func_rx* f = new func_rx;
    f->init(&rx, &tx);
    return f;
  }
  virtual size_t throughput_pps() const override
  {
    size_t sum_pps = 0;
    size_t nb_ports = rx.size();
    for (size_t i=0; i<nb_ports; i++) {
      sum_pps += rx[i].rx_pps();
    }
    return sum_pps;
  }
};

class stage_wk : public stage {
 public:
  std::vector<stageio_rx_ring> rx;
  std::vector<stageio_tx_ring> tx;

  stage_wk(const char* n) : stage(n) { }
  virtual func* allocate() override
  {
    func_wk* f = new func_wk;
    f->init(&rx, &tx);
    return f;
  }
  virtual size_t throughput_pps() const override
  {
    size_t sum_pps = 0;
    size_t nb_ports = rx.size();
    for (size_t i=0; i<nb_ports; i++) {
      sum_pps += rx[i].rx_pps();
    }
    return sum_pps;
  }
};

class stage_tx : public stage {
 public:
  std::vector<stageio_rx_ring> rx;
  std::vector<stageio_tx_port> tx;

  stage_tx(const char* n) : stage(n) { }
  virtual func* allocate() override
  {
    func_tx* f = new func_tx;
    f->init(&rx, &tx);
    return f;
  }
  virtual size_t throughput_pps() const override
  {
    size_t sum_pps = 0;
    size_t nb_ports = rx.size();
    for (size_t i=0; i<nb_ports; i++) {
      sum_pps += rx[i].rx_pps();
    }
    return sum_pps;
  }
};
#endif

