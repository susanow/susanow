
#pragma once
#include <ssnlib_sys.h>

namespace ssnlib {

using Rxq    = Rxq_interface<Ring_dpdk>;
using Txq    = Txq_interface<Ring_dpdk>;
using Port   = Port_interface<Rxq, Txq>;
using Cpu    = Cpu_interface;
using System = System_interface<Cpu, Port>;

} /* namespace ssnlib */
