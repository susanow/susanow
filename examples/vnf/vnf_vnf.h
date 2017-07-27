
#pragma once


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
    wk->add_input_ring(ring_poswk[0]);
    wk->add_input_ring(ring_poswk[1]);
    wk->add_output_port(0);
    wk->add_output_port(1);
    stages.push_back(tx);
  }
};


