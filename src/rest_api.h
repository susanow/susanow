
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



/*
 * @brief REST API to controll nfvi
 * @param sys ssn_nfvi pointer
 * @details
 *   User can extend this function to add new REST-API easily
 */
int rest_api_thread(ssn_nfvi* nfviptr)
{
  using std::string;
  using slankdev::format;
  ssn_nfvi& nfvi = *nfviptr;
  const std::vector<ssn_vnf*>& vnfs = nfvi.get_vnfs();
  const std::vector<ssn_vnf_port*>& ports = nfvi.get_ports();

  crow::SimpleApp app;
  app.loglevel(crow::LogLevel::Critical);

  CROW_ROUTE(app,"/") ( []() {
      crow::json::wvalue x;
      x["result"] = responce_info(true, "");
      crow::json::wvalue child;
      child["ports"] = "ports operation";
      child["vnfs"]  = "vnfs operation";
      x["cmds"] = std::move(child);
      return x;
  });

  CROW_ROUTE(app,"/system") ( [&nfvi, &vnfs, &ports]() {

      crow::json::wvalue x_mempool;
      rte_mempool* mp = nfvi.get_mp();
      x_mempool["ptr"] = format("%p", mp);
      x_mempool["avail_cnt"] = rte_mempool_avail_count(mp);
      x_mempool["free_cnt"] = rte_mempool_in_use_count(mp);

      crow::json::wvalue x_root;
      x_root["result"] = responce_info(true, "");
      x_root["n_vnf"]  = vnfs.size();
      x_root["n_port"] = ports.size();
      x_root["mempool"] = std::move(x_mempool);
      return x_root;

  });

  CROW_ROUTE(app,"/ports") ( [&nfvi, &vnfs, &ports]() {
      /*
       * "n_port" : 1
       * "port0"  : {
       *    "n_rxq"  : 4,
       *    "n_txq"  : 4,
       *    "n_rxa"  : 2,
       *    "n_txa"  : 2,
       *    "perfred": 0.8
       * }
       */
      crow::json::wvalue x;
      size_t n_port = ports.size();
      x["result"] = responce_info(true, "");
      x["n_port"] = n_port;
      for (size_t i=0; i<n_port; i++) {
        const ssn_vnf_port* port = ports.at(i);
        x[std::to_string(i)] = std::move(vnf_port_info(port));
      }
      return x;
  });

  CROW_ROUTE(app,"/vnfs") ( [&nfvi, &vnfs, &ports]() {
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
      size_t n_vnf = vnfs.size();
      x["n_vnf"] = n_vnf;
      for (size_t i=0; i<n_vnf; i++) {
        const ssn_vnf* vnf = vnfs.at(i);
        x[std::to_string(i)] = std::move(vnf_info(vnf));
      }
      return x;
  });

  CROW_ROUTE(app, "/vnfs/<str>/coremask") .methods("PUT"_method, "DELETE"_method)
  ([&nfvi](const crow::request& req, std::string str) {

    if (req.method == crow::HTTPMethod::PUT) {

      crow::json::wvalue x;
      /*
       * Find VNF by name...
       */
      auto* vnf = nfvi.find_vnf(str.c_str());
      if (!vnf) {
        x["result"] = responce_info(false, "not found vnf");
        return x;
      }

      /*
       * Check VNF State. if state is running then, return with err.
       */
      if (vnf->is_running()) {
        x["result"] = responce_info(false, "vnf still running");
        return x;
      }

      /*
       * {
       *    {
       *      "blockid"  : 0,
       *      "coremask" : 4
       *    }
       * }
       */
      auto req_json = crow::json::load(req.body);
      size_t   blockid  = req_json["blockid"].i();
      uint32_t coremask = req_json["coremask"].i();
      vnf->set_coremask(blockid, coremask);

      x["result"] = responce_info(true, "found vnf");
      x["vnf"]    = std::move(vnf_info(vnf));
      return x;

    } else if (req.method == crow::HTTPMethod::DELETE) {

      crow::json::wvalue x;
      /*
       * Find VNF by name...
       */
      auto* vnf = nfvi.find_vnf(str.c_str());
      if (!vnf) {
        x["result"] = responce_info(false, "not found vnf");
        return x;
      }

      /*
       * Check VNF State. if state is running then, return with err.
       */
      if (vnf->is_running()) {
        x["result"] = responce_info(false, "vnf is still runing");
        return x;
      }

      vnf->reset_allport_acc();
      x["result"] = responce_info(true, "found vnf");
      x["vnf"]    = std::move(vnf_info(vnf));
      return x;

    }
  });


  CROW_ROUTE(app, "/vnfs/<str>/running") .methods("PUT"_method,"GET"_method,"DELETE"_method)
  ([&nfvi](const crow::request& req, std::string str) {

    if (req.method == crow::HTTPMethod::GET) {

      printf("GET name: %s\n", str.c_str());
      crow::json::wvalue x;
      auto* vnf = nfvi.find_vnf(str.c_str());
      if (!vnf) {
        x["result"] = responce_info(false, "not found vnf");
        return x;
      }
      x["result"] = responce_info(true, "found");
      x["vnf"]    = std::move(vnf_info(vnf));
      return x;

    } else if (req.method == crow::HTTPMethod::PUT) {

      crow::json::wvalue x;
      printf("PUT name: %s\n", str.c_str());

      /*
       * Find VNF by name...
       */
      auto* vnf = nfvi.find_vnf(str.c_str());
      if (!vnf) {
        x["result"] = responce_info(false, "not found vnf");
        return x;
      }

      /*
       * Check VNF State. if state is running then, return with err.
       */
      if (vnf->is_running()) {
        x["result"] = responce_info(false, "already running");
        return x;
      }

      vnf->deploy();
      x["result"] = responce_info(true, "found");
      x["vnf"]    = std::move(vnf_info(vnf));
      return x;

    } else if (req.method == crow::HTTPMethod::DELETE) {

      printf("DELETE name: %s\n", str.c_str());
      crow::json::wvalue x;

      /*
       * Find VNF by name...
       */
      ssn_vnf* vnf = nfvi.find_vnf(str.c_str());
      if (!vnf) {
        x["result"] = responce_info(false, "not found");
        return x;
      }

      /*
       * Check VNF State. if state is running then, return with err.
       */
      if (!vnf->is_running()) {
        x["result"] = responce_info(false, "vnf is not running");
        return x;
      }

      vnf->undeploy();
      x["result"] = responce_info(true, "found");
      x["vnf"]    = std::move(vnf_info(vnf));
      return x;
    }

  });

  CROW_ROUTE(app, "/ports/<str>/config") .methods("GET"_method)
  ([&nfvi](const crow::request& req, std::string str) {

    crow::json::wvalue x;
    auto* port = nfvi.find_port(str.c_str());
    if (!port) {
      x["result"] = responce_info(false, "port not found");
      return x;
    }
    x["result"] = responce_info(true, "found");
    x["port"]   = std::move(vnf_port_info(port));
    return x;

  });

  app.port(8888).run();

} /* rest_api_thread(ssn_nfvi& nfvi) */


