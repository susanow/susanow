
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include "crow_all.h"
#include <slankdev/string.h>

static inline crow::json::wvalue responce_success(bool b)
{
  crow::json::wvalue x;
  x["Success"] = b?"True":"False";
  return x;
}

#if 0
static inline
crow::json::wvalue vnf_port_info(ssn_vnf_port& port)
{
  using std::string;
  using slankdev::format;

  const size_t n_rxq   = port.n_rxq;
  const size_t n_txq   = port.n_txq;
  const size_t n_rxa   = port.n_rxacc;
  const size_t n_txa   = port.n_txacc;
  const double perfred = port.perf_reduction;

  crow::json::wvalue x;
  x["name"    ] = port.name;
  x["n_rxq"   ] = n_rxq;
  x["n_txq"   ] = n_txq;
  x["n_rxa"   ] = n_rxa;
  x["n_txa"   ] = n_txa;
  x["perfred" ] = perfred;
  return x;
}

static inline
crow::json::wvalue vnf_info(ssn_vnf& vnf)
{
  using std::string;
  using slankdev::format;

  string name    = vnf.name;
  size_t n_port  = vnf.ports.size();
  size_t n_block = vnf.blocks.size();
  uint64_t cmask = vnf.coremask;
  bool running   = vnf.running;

  crow::json::wvalue x;
  x["name"]    = name;
  x["n_port"]  = n_port;
  x["n_block"] = n_block;
  x["cmask"]   = slankdev::format("%08x", cmask);
  x["running"] = running;

  crow::json::wvalue x_ports;
  for (size_t i=0; i<n_port; i++) {
    string s = format("port%zd", i);
    x_ports[s] = vnf_port_info(*vnf.ports[i]);
  }
  x["ports"] = std::move(x_ports);
  return x;
}
#endif

