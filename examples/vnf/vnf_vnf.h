
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

    stage* rx = new stage("rx", func_rx);
#if 0
    rx->rx.resize(2);
    rx->rx[0].set(0);
    rx->rx[1].set(1);
    rx->tx.resize(2);
    rx->tx[0].set(ring_prewk[0]);
    rx->tx[1].set(ring_prewk[1]);
#else
    rx->add_input_port(0, 0);
    rx->add_input_port(1, 1);
    rx->add_output_ring(ring_prewk[0]);
    rx->add_output_ring(ring_prewk[1]);
#endif
    stages.push_back(rx);

    stage* wk = new stage("wk", func_wk);
#if 0
    wk->rx.resize(2);
    wk->rx[0].set(ring_prewk[0]);
    wk->rx[1].set(ring_prewk[1]);
    wk->tx.resize(2);
    wk->tx[0].set(ring_poswk[0]);
    wk->tx[1].set(ring_poswk[1]);
#else
    wk->in_resize(2);
    wk->in_set_ring(0, ring_prewk[0]);
    wk->in_set_ring(1, ring_prewk[1]);
    wk->out_resize(2);
    wk->out_set_ring(0, ring_poswk[0]);
    wk->out_set_ring(1, ring_poswk[1]);
#endif
    stages.push_back(wk);

    stage* tx = new stage("tx", func_tx);
#if 0
    tx->rx.resize(2);
    tx->rx[0].set(ring_poswk[0]);
    tx->rx[1].set(ring_poswk[1]);
    tx->tx.resize(2);
    tx->tx[0].set(0);
    tx->tx[1].set(1);
#else
    wk->in_resize(2);
    wk->in_set_ring(0, ring_poswk[0]);
    wk->in_set_ring(1, ring_poswk[1]);
    wk->out_resize(2);
    wk->out_set_port(0, 0);
    wk->out_set_port(1, 1);
#endif
    stages.push_back(tx);
  }
};


