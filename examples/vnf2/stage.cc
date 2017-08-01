

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
#include "nfvi.h"


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
 * stage class member function implementation
 */
void stage::add_input_ring(ssn_ring* ring_ptr)
{
  stageio_rx* ringio = new stageio_rx;
  ringio->set(ring_ptr);
  rx.push_back(ringio);
}
void stage::add_output_ring(ssn_ring* r)
{
  stageio_tx* ringio = new stageio_tx;
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

void stage::add_nic(vnic* nic)
{
  add_input_ring(nic->rx);
  add_output_ring(nic->tx);
}



