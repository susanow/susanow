
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string>
#include <crow.h>
#include <slankdev/string.h>
#include <slankdev/exception.h>
#include <ssn_nfvi.h>
#include "wip.h"


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

void add_route_about_vnf_operation(ssn_nfvi& nfvi, crow::SimpleApp& app)
{
  using std::string;
  using slankdev::format;

  CROW_ROUTE(app,"/vnfcatalog") ( [&nfvi]() {
      /*
       * "n_vcat" : 2
       * "0"  : {
       *    "name" : "l2fwd1b",
       *    "allocator" : "0x033ef1" // pointer
       * },
       * "1" : {
       *    "name" : "l2fwd2b",
       *    "allocator" : "0x033ef2" // pointer
       * }
       */
      crow::json::wvalue x_root;
      const size_t n_vcat = nfvi.get_vcat().size();
      x_root["result"] = responce_info(true, "");
      x_root["n_vcat"] = n_vcat;
      crow::json::wvalue x_vcat;
      for (size_t i=0; i<n_vcat; i++) {
        x_vcat["name"] = nfvi.get_vcat()[i].name;
        x_vcat["allocator"] = format("%p", nfvi.get_vcat()[i].allocator);
        x_root[std::to_string(i)] = std::move(x_vcat);
      }
      return x_root;
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
}

void add_route_about_port_operation(ssn_nfvi& nfvi, crow::SimpleApp& app)
{
  using std::string;
  using slankdev::format;

  CROW_ROUTE(app, "/portcatalog").methods("GET"_method)
  ([&nfvi](const crow::request& req) {

      assert(  (req.method == crow::HTTPMethod::GET)  );

      if (req.method == crow::HTTPMethod::GET) {

        /*
         * "n_vcat" : 2
         * "0"  : {
         *    "name" : "pci",
         *    "allocator" : "0x033ef1" // pointer
         * },
         * "1" : {
         *    "name" : "l2fwd2b",
         *    "allocator" : "0x033ef2" // pointer
         * }
         */
        crow::json::wvalue x_root;
        const size_t n_pcat = nfvi.get_pcat().size();
        x_root["result"] = responce_info(true, "");
        x_root["n_pcat"] = n_pcat;
        crow::json::wvalue x_pcat;
        for (size_t i=0; i<n_pcat; i++) {
          x_pcat["name"] = nfvi.get_pcat()[i].name;
          x_pcat["allocator"] = format("%p", nfvi.get_pcat()[i].allocator);
          x_root[std::to_string(i)] = std::move(x_pcat);
        }
        return x_root;

      }
  });

  CROW_ROUTE(app,"/ports")
  .methods("GET"_method, "PUT"_method, "DELETE"_method)
  ([&nfvi](const crow::request& req) {

      assert(  (req.method == crow::HTTPMethod::GET)
           ||  (req.method == crow::HTTPMethod::PUT)
           ||  (req.method == crow::HTTPMethod::DELETE)  );

      if (req.method == crow::HTTPMethod::GET) {

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
        size_t n_port = nfvi.get_ports().size();
        x["result"] = responce_info(true, "");
        x["n_port"] = n_port;
        for (size_t i=0; i<n_port; i++) {
          const ssn_vnf_port* port = nfvi.get_ports().at(i);
          x[std::to_string(i)] = std::move(vnf_port_info(port));
        }
        return x;

      } else if (req.method == crow::HTTPMethod::PUT) {

        /* Alloc new port */
        /* # User send JSON
         * {
         *    "cname"      : "pci"
         *    "iname" : "pci0"
         *    "options" : {
         *      ...
         *      "pciaddr" : "0000:01:00.1"
         *      ""
         *    }
         * }
         */
        auto req_json = crow::json::load(req.body);
        std::string cname   = req_json["cname"].s();
        std::string iname   = req_json["iname"].s();

        if (cname == "tap") {

          /*
           * "options" :
           *    {
           *       "ifname" : "eth0"
           *    }
           */
          std::string ifname = req_json["options"]["ifname"].s();
          rte_mempool* mp = nfvi.get_mp();
          ssn_portalloc_tap_arg arg = { mp, ifname };
          nfvi.port_alloc_from_catalog(cname.c_str(), iname.c_str(), &arg);

          crow::json::wvalue x_root;
          x_root["result"] = responce_info(true, "");
          return x_root;

        } else if (cname == "pci") {

          /*
           * "options" :
           *    {
           *       "pciaddr" : "0000:01:00.1"
           *    }
           */
          std::string pciaddr = req_json["options"]["pciaddr"].s();
          rte_mempool* mp = nfvi.get_mp();
          ssn_portalloc_pci_arg arg = { mp, pciaddr };
          nfvi.port_alloc_from_catalog(cname.c_str(), iname.c_str(), &arg);

          crow::json::wvalue x_root;
          x_root["result"] = responce_info(true, "");
          return x_root;

        } else if (cname == "virt") {

          /*
           * "options" :
           *    {
           *    }
           */
          ssn_portalloc_virt_arg arg;
          nfvi.port_alloc_from_catalog(cname.c_str(), iname.c_str(), &arg);

          crow::json::wvalue x_root;
          x_root["result"] = responce_info(true, "");
          return x_root;

        } else {

          crow::json::wvalue x_root;
          x_root["result"] = responce_info(false, "catalog name is invalid");
          return x_root;

        }

      } else if (req.method == crow::HTTPMethod::DELETE) {

        /* Delete port */
        /* # User send JSON
         * {
         *    "iname" : "pci0"
         * }
         */
        auto req_json = crow::json::load(req.body);
        std::string iname = req_json["iname"].s();
        printf("iname: %s \n", iname.c_str());

        try {

          auto* port = nfvi.find_port(iname.c_str());
          printf("found: %p \n", port);
          nfvi.del_port(port);
          crow::json::wvalue x_root;
          x_root["result"] = responce_info(true, "");
          return x_root;

        } catch (std::exception& e) {

          printf("not found err: %s", e.what());
          crow::json::wvalue x_root;
          x_root["result"] = responce_info(false, "some exception throwed");
          return x_root;

        }
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
}

int rest_api_thread(ssn_nfvi* nfviptr)
{
  using std::string;
  using slankdev::format;
  ssn_nfvi& nfvi = *nfviptr;

  crow::SimpleApp app;
  app.loglevel(crow::LogLevel::Critical);
  app.loglevel(crow::LogLevel::Debug);

  CROW_ROUTE(app,"/") ( []() {
      crow::json::wvalue x;
      x["result"] = responce_info(true, "");
      crow::json::wvalue child;
      child["ports"] = "ports operation";
      child["vnfs"]  = "vnfs operation";
      x["cmds"] = std::move(child);
      return x;
  });

  CROW_ROUTE(app,"/system") ( [&nfvi]() {

      crow::json::wvalue x_root;
      x_root["result"] = responce_info(true, "");
      x_root["n_vnf"]  = nfvi.get_vnfs().size();
      x_root["n_port"] = nfvi.get_ports().size();
      x_root["n_vcat"] = nfvi.get_vcat().size();
      x_root["n_pcat"] = nfvi.get_pcat().size();
      return x_root;

  });

  add_route_about_port_operation(nfvi, app);
  add_route_about_vnf_operation(nfvi, app);
  add_route_work_in_progress(nfvi, app);

  app.port(8888).run();

} /* rest_api_thread(ssn_nfvi& nfvi) */


