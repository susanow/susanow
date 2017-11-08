
#pragma once

#include <stdio.h>
#include <string>
#include <slankdev/string.h>
#include <slankdev/exception.h>
#include <crow.h>

#include "ssn_nfvi.h"
#include "json_macro.h"


int rest_api_thread(ssn_nfvi* sys)
{
  crow::SimpleApp app;

  CROW_ROUTE(app,"/") ( []() {
      printf("access /\n");
      crow::json::wvalue x;
      x["result"] = responce_info(true, "");
      crow::json::wvalue child;
      child["ports"] = "ports operation";
      child["vnfs"]  = "vnfs operation";
      x["cmds"] = std::move(child);
      return x;
  });

  CROW_ROUTE(app,"/ports") ( [&sys]() {
      using std::string;
      using slankdev::format;
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
      size_t n_port = sys->ports.size();
      x["result"] = responce_info(true, "");
      x["n_port"] = n_port;
      for (size_t i=0; i<n_port; i++) {
        ssn_vnf_port* port = sys->ports.at(i);
        x[std::to_string(i)] = std::move(vnf_port_info(port));
      }
      return x;
  });

  CROW_ROUTE(app,"/vnfs") ( [&sys]() {
      using std::string;
      using slankdev::format;
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
      size_t n_vnf = sys->vnfs.size();
      x["n_vnf"] = n_vnf;
      for (size_t i=0; i<n_vnf; i++) {
        ssn_vnf* vnf = sys->vnfs.at(i);
        x[std::to_string(i)] = std::move(vnf_info(vnf));
      }
      return x;
  });

  CROW_ROUTE(app, "/vnfs/<str>/coremask") .methods("PUT"_method, "DELETE"_method)
  ([&sys](const crow::request& req, std::string str) {

    if (req.method == crow::HTTPMethod::PUT) {

      crow::json::wvalue x;
      /*
       * Find VNF by name...
       */
      auto* vnf = sys->find_vnf(str.c_str());
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
      auto* vnf = sys->find_vnf(str.c_str());
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
  ([&sys](const crow::request& req, std::string str) {

    if (req.method == crow::HTTPMethod::GET) {

      printf("GET name: %s\n", str.c_str());
      crow::json::wvalue x;
      auto* vnf = sys->find_vnf(str.c_str());
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
      auto* vnf = sys->find_vnf(str.c_str());
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
      ssn_vnf* vnf = sys->find_vnf(str.c_str());
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
  ([&sys](const crow::request& req, std::string str) {

    crow::json::wvalue x;
    auto* port = sys->find_port(str.c_str());
    if (!port) {
      x["result"] = responce_info(false, "port not found");
      return x;
    }
    x["result"] = responce_info(true, "found");
    x["port"]   = std::move(vnf_port_info(port));
    return x;

  });

  app.loglevel(crow::LogLevel::Debug);
  app.port(8888).run();

} /* rest_api_thread(ssn_nfvi* sys) */


