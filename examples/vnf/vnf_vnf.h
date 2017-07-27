
#pragma once


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

    stage_rx* rx = new stage_rx("rx");
    rx->rx.resize(2);
    rx->rx[0].set(0);
    rx->rx[1].set(1);
    rx->tx.resize(2);
    rx->tx[0].set(ring_prewk[0]);
    rx->tx[1].set(ring_prewk[1]);
    stages.push_back(rx);

    stage_wk* wk = new stage_wk("wk");
    wk->rx.resize(2);
    wk->rx[0].set(ring_prewk[0]);
    wk->rx[1].set(ring_prewk[1]);
    wk->tx.resize(2);
    wk->tx[0].set(ring_poswk[0]);
    wk->tx[1].set(ring_poswk[1]);
    stages.push_back(wk);

    stage_tx* tx = new stage_tx("tx");
    tx->rx.resize(2);
    tx->rx[0].set(ring_poswk[0]);
    tx->rx[1].set(ring_poswk[1]);
    tx->tx.resize(2);
    tx->tx[0].set(0);
    tx->tx[1].set(1);
    stages.push_back(tx);
  }
};


