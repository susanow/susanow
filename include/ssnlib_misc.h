
#pragma once

#include <rte_launch.h>

#include <slankdev/vty.h>

#include <ssnlib_sys.h>


static inline System* get_sys(slankdev::shell* sh)
{
  return reinterpret_cast<System*>(sh->user_ptr);
}

static inline std::string get_cpustate(uint32_t lcoreid)
{
  rte_lcore_state_t s = rte_eal_get_lcore_state(lcoreid);
  switch (s) {
    case WAIT    : return "WAIT    ";
    case RUNNING : return "RUNNING ";
    case FINISHED: return "FINISHED";
    default:
                   throw slankdev::exception("UNKNOWN STATE");
  }
}

