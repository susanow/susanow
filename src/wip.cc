

#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string>
#include <crow.h>
#include <slankdev/string.h>
#include <slankdev/exception.h>
#include <ssn_nfvi.h>
#include "json_macro.h"

void add_route_work_in_progress (ssn_nfvi& nfvi, crow::SimpleApp& app)
{
  using std::string;
  using slankdev::format;

  CROW_ROUTE(app,"/vnfs")
  .methods("GET"_method, "PUT"_method, "DELETE"_method)
  ([&nfvi](const crow::request& req) {

      assert(  (req.method == crow::HTTPMethod::GET)
           ||  (req.method == crow::HTTPMethod::PUT)
           ||  (req.method == crow::HTTPMethod::DELETE)  );

      if (req.method == crow::HTTPMethod::GET) {

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

      } else if (req.method == crow::HTTPMethod::PUT) {

        /*
         * {
         *    "cname" : "l2fwd1b",
         *    "iname" : "vnf0"
         * }
         */
        auto req_json = crow::json::load(req.body);
        std::string cname = req_json["cname"].s();
        std::string iname = req_json["iname"].s();

        try {

          nfvi.vnf_alloc_from_catalog(cname.c_str(), iname.c_str());
          crow::json::wvalue x_root;
          x_root["result"] = responce_info(true, "");
          return x_root;

        } catch (std::exception& e) {

          crow::json::wvalue x_root;
          x_root["result"] = responce_info(false, e.what());
          return x_root;

        }

      } else if (req.method == crow::HTTPMethod::DELETE) {

        /*
         * {
         *    "iname" : "vnf0"
         * }
         */
        auto req_json = crow::json::load(req.body);
        std::string iname = req_json["iname"].s();

        try {

          ssn_vnf* vnf = nfvi.find_vnf(iname.c_str());
          nfvi.del_vnf(vnf);
          crow::json::wvalue x_root;
          x_root["result"] = responce_info(true, "");
          return x_root;

        } catch (std::exception& e) {

          crow::json::wvalue x_root;
          x_root["result"] = responce_info(false, e.what());
          return x_root;

        }
      }

  });
}


