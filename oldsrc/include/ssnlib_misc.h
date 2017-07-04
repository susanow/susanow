
#pragma once

#include <rte_launch.h>

#include <slankdev/vty.h>

#include <ssnlib_sys.h>


static inline System* get_sys(slankdev::shell* sh)
{
  return reinterpret_cast<System*>(sh->user_ptr);
}

