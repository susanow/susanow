

#include <string>
#include <vector>
#include <stdio.h>

#include <slankdev/util.h>
#include <slankdev/exception.h>
#include <slankdev/extra/dpdk.h>

#include <ssn_common.h>
#include <ssn_sys.h>
#include <ssn_timer.h>
#include <ssn_native_thread.h>
#include <ssn_green_thread.h>
#include <ssn_port_stat.h>
#include <ssn_port.h>
#include <ssn_log.h>
#include <ssn_ring.h>

#include "func.h"
#include "stage.h"



static inline void _func_spawner(void* arg)
{
  func* f = reinterpret_cast<func*>(arg);
  f->poll_exe();
}

static inline ssize_t get_free_lcore_id()
{
  size_t nb_lcores = ssn_lcore_count();
  for (size_t i=1; i<nb_lcores; i++) {
    auto s = ssn_get_lcore_state(i);
    if (s == SSN_LS_WAIT) return i;
  }
  return -1;
}





/*
 * stageio class member function implementation
 */
size_t stageio_rx_ring::rx_burst(rte_mbuf** obj_table, size_t n)
{ return ring->deq_bulk((void**)obj_table, n); }
size_t stageio_rx_ring::rx_pps() const { return ring->opps; }

int stageio_tx_ring::tx_shot(rte_mbuf* obj) { return ring->enq((void*)obj); }
size_t stageio_tx_ring::tx_burst(rte_mbuf** obj_table, size_t n)
{ return ring->enq_bulk((void* const*)obj_table, n); }

void stageio_rx_port::set(size_t p) { pid = p; }
size_t stageio_rx_port::rx_burst(rte_mbuf** obj_table, size_t n)
{ return rte_eth_rx_burst(pid, 0, obj_table, n); }
size_t stageio_rx_port::rx_pps() const { return  ssn_port_stat_get_cur_rx_pps(pid); }

void stageio_tx_port::set(size_t p) { pid = p; }
int stageio_tx_port::tx_shot(rte_mbuf* obj)
{ throw slankdev::exception("not imple"); }
size_t stageio_tx_port::tx_burst(rte_mbuf** obj_table, size_t n)
{ return rte_eth_tx_burst(pid, 0, obj_table, n); }




/*
 * stage class member function implementation
 */
void stage::add_input_port(size_t pid)
{
  stageio_rx_port* port = new stageio_rx_port;
  port->set(pid);
  rx.push_back(port);
}
void stage::add_input_ring(ssn_ring* ring_ptr)
{
  stageio_rx_ring* ringio = new stageio_rx_ring;
  ringio->set(ring_ptr);
  rx.push_back(ringio);
}
void stage::add_output_port(size_t pid)
{
  stageio_tx_port* port = new stageio_tx_port;
  port->set(pid);
  tx.push_back(port);
}
void stage::add_output_ring(ssn_ring* r)
{
  stageio_tx_ring* ringio = new stageio_tx_ring;
  ringio->set(r);
  tx.push_back(ringio);
}
size_t stage::mux() const { return mux_; }
void stage::inc()
{
  func* f = alloc_fn(this);
  ssize_t lcore_id = get_free_lcore_id();
  if (lcore_id < 0) throw slankdev::exception("no lcore");
  funcs.push_back(f);
  ssn_native_thread_launch(_func_spawner, f, lcore_id);
  mux_ ++ ;
}
void stage::dec()
{
  size_t idx = funcs.size()-1;
  funcs[idx]->stop();
  funcs.erase(funcs.begin() + idx);
  mux_ -- ;
}
size_t stage::throughput_pps() const
{
  size_t sum_pps = 0;
  size_t nb_ports = rx.size();
  for (size_t i=0; i<nb_ports; i++) {
    sum_pps += rx[i]->rx_pps();
  }
  return sum_pps;
}




