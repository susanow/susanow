
/*
 * MIT License
 *
 * Copyright (c) 2017 Susanow
 * Copyright (c) 2017 Hiroki SHIROKURA
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include <stdio.h>
#include <string>
#include <slankdev/string.h>
#include <slankdev/exception.h>

#include "crow_all.h"
#include "ssn_interface.h"
#include "json_macro.h"


int rest_api_thread(void* arg_)
{
  ssn_nfvi* sys = reinterpret_cast<ssn_nfvi*>(arg_);
  crow::SimpleApp app;

  CROW_ROUTE(app,"/") ( [&sys]() {
      printf("access /\n");
      crow::json::wvalue x = responce_success(true);
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
      x["n_port"] = n_port;
      for (size_t i=0; i<n_port; i++) {
        auto& port = (sys->ports[i]);
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
      size_t n_vnf = sys->vnfs.size();
      x["n_vnf"] = n_vnf;
      for (size_t i=0; i<n_vnf; i++) {
        auto& vnf = sys->vnfs[i];
        x[std::to_string(i)] = std::move(vnf_info(vnf));
      }
      return x;
  });

  CROW_ROUTE(app, "/vnfs/<str>/running") .methods("PUT"_method,"GET"_method,"DELETE"_method)
  ([&sys](const crow::request& req, std::string str) {

    if (req.method == crow::HTTPMethod::GET) {

      crow::json::wvalue x;
      auto* vnf = sys->find_vnf(str.c_str());
      if (!vnf) {
        x["Success"] = false;
        x["msg"]     = "not found vnf";
        return x;
      }
      x["Success"] = true;
      x["msg"]     = "found!!!";
      x["vnf"]     = std::move(vnf_info(*vnf));
      return x;

    } else if (req.method == crow::HTTPMethod::PUT) {

      crow::json::wvalue x;
      auto* vnf = sys->find_vnf(str.c_str());
      if (!vnf) {
        x["Success"] = false;
        x["msg"]     = "not found vnf";
        return x;
      }
      auto req_json = crow::json::load(req.body);
      uint64_t coremask = req_json["coremask"].i();

      vnf->running = true;
      vnf->coremask = coremask;
      x["Success"] = true;
      x["msg"]     = "found!!!";
      x["vnf"]     = std::move(vnf_info(*vnf));
      return x;

    } else if (req.method == crow::HTTPMethod::DELETE) {

      printf("DELETE name: %s\n", str.c_str());
      crow::json::wvalue x;
      auto* vnf = sys->find_vnf(str.c_str());
      if (!vnf) {
        x["Success"] = false;
        x["msg"]     = "not found vnf";
        return x;
      }
      vnf->running = false;
      vnf->coremask = 0;
      x["Success"] = true;
      x["msg"]     = "found!!!";
      x["vnf"]     = std::move(vnf_info(*vnf));
      return x;
    }

  });

  CROW_ROUTE(app, "/ports/<str>/config") .methods("GET"_method)
  ([&sys](const crow::request& req, std::string str) {

    crow::json::wvalue x;
    auto* port = sys->find_port(str.c_str());
    if (!port) {
      x["Success"] = false;
      x["msg"]     = "not found port";
      return x;
    }
    x["Success"] = true;
    x["msg"]     = "found!!!";
    x["port"]     = std::move(vnf_port_info(*port));
    return x;

  });

  app.loglevel(crow::LogLevel::Debug);
  app.port(8888).run();
}

int main(int argc, char** argv)
{
  ssn_nfvi sys;
  std::thread rest_server(rest_api_thread, &sys);
  rest_server.join();
}


