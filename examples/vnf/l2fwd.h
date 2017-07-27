
#pragma once
#include "func.h"

class func_rx : public func {
 public:

  func_rx(std::vector<stageio_rx*>& _rx,
          std::vector<stageio_tx*>& _tx) : func(_rx, _tx) {}
  virtual void poll_exe() override
  {
    size_t nb_ports = ssn_dev_count();
    run = true;
    while (run) {
      for (size_t p=0; p<nb_ports; p++) {
        rte_mbuf* mbufs[32];
        size_t recvlen = rx[p]->rx_burst(mbufs, 32);
        if (recvlen == 0) continue;
        size_t enqlen = tx[p]->tx_burst(mbufs, recvlen);
        if (recvlen > enqlen) {
          slankdev::rte_pktmbuf_free_bulk(&mbufs[enqlen], recvlen-enqlen);
        }
      }
    }
  }
  virtual void stop() override { run = false; }
};



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
          // for (size_t j=0; j<100; j++) ; // DELAY
          for (size_t j=0; j<40; j++) ; // DELAY
          int ret = tx[p^1]->tx_shot(mbufs[i]);
          if (ret < 0) rte_pktmbuf_free(mbufs[i]);
        }
      }
    }
  }
  virtual void stop() override { run = false; }
};



class func_tx : public func {
 public:

  func_tx(std::vector<stageio_rx*>& _rx,
          std::vector<stageio_tx*>& _tx) : func(_rx, _tx) {}
  virtual void poll_exe() override
  {
    size_t nb_ports = ssn_dev_count();
    while (run) {
      for (size_t p=0; p<nb_ports; p++) {
        rte_mbuf* mbufs[32];
        size_t deqlen = rx[p]->rx_burst(mbufs, 32);
        if (deqlen == 0) continue;
        size_t sendlen = tx[p]->tx_burst(mbufs, deqlen);
        if (deqlen > sendlen) {
          slankdev::rte_pktmbuf_free_bulk(&mbufs[sendlen], deqlen-sendlen);
        }
      }
    }
  }
  virtual void stop() override { run = false; }
};

func* alloc_func_rx(void* arg)
{
  stage* stg = reinterpret_cast<stage*>(arg);
  func_rx* f = new func_rx(stg->rx, stg->tx);
  return f;
}
func* alloc_func_wk(void* arg)
{
  stage* stg = reinterpret_cast<stage*>(arg);
  func_wk* f = new func_wk(stg->rx, stg->tx);
  return f;
}
func* alloc_func_tx(void* arg)
{
  stage* stg = reinterpret_cast<stage*>(arg);
  func_tx* f = new func_tx(stg->rx, stg->tx);
  return f;
}

class vnf_l2fwd : public vnf {
 public:
  ssn_ring* ring_prewk[2];
  ssn_ring* ring_poswk[2];
  vnf_l2fwd()
  {
    ring_prewk[0] = new ssn_ring("prewk0");
    ring_prewk[1] = new ssn_ring("prewk1");
    ring_poswk[0] = new ssn_ring("poswk0");
    ring_poswk[1] = new ssn_ring("poswk1");

    stage* rx = new stage("rx", alloc_func_rx);
    rx->add_input_port(0);
    rx->add_input_port(1);
    rx->add_output_ring(ring_prewk[0]);
    rx->add_output_ring(ring_prewk[1]);
    stages.push_back(rx);

    stage* wk = new stage("wk", alloc_func_wk);
    wk->add_input_ring(ring_prewk[0]);
    wk->add_input_ring(ring_prewk[1]);
    wk->add_output_ring(ring_poswk[0]);
    wk->add_output_ring(ring_poswk[1]);
    stages.push_back(wk);

    stage* tx = new stage("tx", alloc_func_tx);
    tx->add_input_ring(ring_poswk[0]);
    tx->add_input_ring(ring_poswk[1]);
    tx->add_output_port(0);
    tx->add_output_port(1);
    stages.push_back(tx);
  }
};


