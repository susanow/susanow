

#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string>
#include <crow.h>
#include <slankdev/string.h>
#include <slankdev/exception.h>
#include <ssn_nfvi.h>

/*
 * JSON Macros
 */
namespace {

static inline
crow::json::wvalue responce_info(bool success, const char* msg)
{
  crow::json::wvalue x;
  x["success"] = success;
  x["msg"] = msg;
  return x;
}

static inline
crow::json::wvalue vnf_port_info(const ssn_vnf_port* port)
{
  using std::string;
  using slankdev::format;

  const string name    = port->name;
  const size_t n_rxq   = port->get_n_rxq();
  const size_t n_txq   = port->get_n_txq();
  const size_t n_rxa   = port->get_n_rxacc();
  const size_t n_txa   = port->get_n_txacc();
  const double perfred = port->get_perf_reduction();

  crow::json::wvalue x;
  x["name"    ] = name;
  x["n_rxq"   ] = n_rxq;
  x["n_txq"   ] = n_txq;
  x["n_rxa"   ] = n_rxa;
  x["n_txa"   ] = n_txa;
  x["perfred" ] = perfred;
  return x;
}

static inline
crow::json::wvalue vnf_block_info(const ssn_vnf_block* block)
{
  using std::string;
  using slankdev::format;

  std::string name = block->name;
  uint32_t coremask = block->get_coremask();
  bool running = block->is_running();

  crow::json::wvalue x;
  x["name"] = name;
  x["cmask"] = coremask;
  x["running"] = running;
  return x;
}

static inline
crow::json::wvalue vnf_info(const ssn_vnf* vnf)
{
  using std::string;
  using slankdev::format;

  string name    = vnf->name;
  size_t n_port  = vnf->n_ports();
  size_t n_block = vnf->n_blocks();
  bool running   = vnf->is_running();

  crow::json::wvalue x;
  x["name"]    = name;
  x["n_port"]  = n_port;
  x["n_block"] = n_block;
  x["running"] = running;

  crow::json::wvalue x_blocks;
  for (size_t i=0; i<n_block; i++) {
    string s = format("block%zd", i);
    x_blocks[s] = vnf_block_info(vnf->get_block(i));
  }
  x["blocks"] = std::move(x_blocks);

  crow::json::wvalue x_ports;
  for (size_t i=0; i<n_port; i++) {
    string s = format("port%zd", i);
    x_ports[s] = vnf_port_info(vnf->get_port(i));
  }
  x["ports"] = std::move(x_ports);
  return x;
}

} /* namespace */

void add_route_work_in_progress (ssn_nfvi& nfvi, crow::SimpleApp& app)
{
  using std::string;
  using slankdev::format;

  CROW_ROUTE(app,"/vnfs")
  .methods("GET"_method, "PUT"_method, "DELETE"_method)
  ([&nfvi](const crow::request& req) {
      /*
       * "n_vnf" : 1,
       * "0" : {
       *    "name"     : "vnf0",
       *    "n_port"   : 2,
       *    "n_block"  : 1,
       *    "cmask"    : 0x0000,
       *    "ports" : {
       *      "port0" : {
       *         "n_rxq"  : 4,
       *         "n_txq"  : 4,
       *         "n_rxa"  : 2,
       *         "n_txa"  : 2,
       *         "perfred": 0.8
       *      },
       *      "port1" : {
       *         "n_rxq"  : 4,
       *         "n_txq"  : 4,
       *         "n_rxa"  : 2,
       *         "n_txa"  : 2,
       *         "perfred": 0.8
       *      }
       *    }
       * }
       */
      crow::json::wvalue x;
      x["result"] = responce_info(true, "");
      size_t n_vnf = nfvi.get_vnfs().size();
      x["n_vnf"] = n_vnf;
      for (size_t i=0; i<n_vnf; i++) {
        const ssn_vnf* vnf = nfvi.get_vnfs().at(i);
        x[std::to_string(i)] = std::move(vnf_info(vnf));
      }
      return x;
  });

}


