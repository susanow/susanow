
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string>
#include <crow.h>
#include <slankdev/string.h>
#include <slankdev/exception.h>
#include <ssn_nfvi.h>
#include "json_macro.h"


namespace {

using std::string;
using slankdev::format;


void addroute__(ssn_nfvi& nfvi, crow::SimpleApp& app)
{
  CROW_ROUTE(app,"/")
  .methods("GET"_method)
  ([&nfvi]() {
      crow::json::wvalue x_root;
      x_root["result"] = responce_info(true, "");
      x_root["n_vnf"]  = nfvi.get_vnfs().size();
      x_root["n_port"] = nfvi.get_ports().size();
      x_root["n_vcat"] = nfvi.get_vcat().size();
      x_root["n_pcat"] = nfvi.get_pcat().size();
      return x_root;
  });
}

void addroute__vnfs(ssn_nfvi& nfvi, crow::SimpleApp& app)
{
  CROW_ROUTE(app,"/vnfs")
  .methods("GET"_method)
  ([&nfvi](const crow::request& req) {

      assert( req.method == crow::HTTPMethod::GET );
      if (req.method == crow::HTTPMethod::GET) {

        crow::json::wvalue x;
        x["result"] = responce_info(true, "");
        size_t n_vnf = nfvi.get_vnfs().size();
        x["n_vnf"] = n_vnf;
        for (size_t i=0; i<n_vnf; i++) {
          const ssn_vnf* vnf = nfvi.get_vnfs().at(i);
          x[std::to_string(i)] = std::move(vnf_info(vnf));
        }
        return x;

      }
  });
}

void addroute__vnfs_NAME(ssn_nfvi& nfvi, crow::SimpleApp& app)
{
  CROW_ROUTE(app,"/vnfs/<str>")
  .methods("GET"_method, "POST"_method, "DELETE"_method)
  ([&nfvi](const crow::request& req, std::string iname) {

      assert(  (req.method == crow::HTTPMethod::GET   )
           ||  (req.method == crow::HTTPMethod::POST  )
           ||  (req.method == crow::HTTPMethod::DELETE)  );

      if (req.method == crow::HTTPMethod::GET) {

        const ssn_vnf* vnf = nfvi.find_vnf(iname.c_str());
        if (!vnf) {
          crow::json::wvalue x;
          x["result"] = responce_info(false, "vnf not found");
          return x;
        }
        crow::json::wvalue x;
        x["result"] = responce_info(true, "found");
        x["vnf"] = std::move(vnf_info(vnf));
        return x;

      } else if (req.method == crow::HTTPMethod::POST) {

        auto req_json = crow::json::load(req.body);
        std::string cname = req_json["cname"].s();

        ssn_vnf* vnf = nfvi.vnf_alloc_from_catalog(cname.c_str(), iname.c_str());
        if (!vnf) {
          crow::json::wvalue x_root;
          x_root["result"] = responce_info(false, "invalid cname or iname");
          return x_root;
        }
        crow::json::wvalue x_root;
        x_root["result"] = responce_info(true, "");
        return x_root;

      } else if (req.method == crow::HTTPMethod::DELETE) {

        ssn_vnf* vnf = nfvi.find_vnf(iname.c_str());
        if (!vnf) {
          crow::json::wvalue x_root;
          x_root["result"] = responce_info(false, "iname is invalid");
          return x_root;
        }

        if (vnf->is_deletable() == false) {
          crow::json::wvalue x_root;
          x_root["result"] = responce_info(false, "vnf is not deletable");
          return x_root;
        }

        nfvi.del_vnf(vnf);
        crow::json::wvalue x_root;
        x_root["result"] = responce_info(true, "");
        return x_root;
      }

  });
}

void addroute__vnfs_NAME_ports_PORTID(ssn_nfvi& nfvi, crow::SimpleApp& app)
{
  CROW_ROUTE(app, "/vnfs/<str>/ports/<int>")
  .methods("PUT"_method, "DELETE"_method)
  ([&nfvi](const crow::request& req, std::string iname, int pid) {
   try {

      assert(  (req.method == crow::HTTPMethod::PUT   )
           ||  (req.method == crow::HTTPMethod::DELETE)  );

      ssn_vnf* vnf = nfvi.find_vnf(iname.c_str());
      if (!vnf) {
        crow::json::wvalue x_root;
        x_root["result"] = responce_info(false, "not found vnf");
        return x_root;
      }
      if (pid >= vnf->n_ports()) {
        crow::json::wvalue x_root;
        x_root["result"] = responce_info(false, "invalid prot id");
        return x_root;
      }

      if (req.method == crow::HTTPMethod::PUT) {

        auto req_json = crow::json::load(req.body);
        std::string pname = req_json["pname"].s();
        ssn_vnf_port* port = nfvi.find_port(pname.c_str());
        if (!port) {
          crow::json::wvalue x_root;
          x_root["result"] = responce_info(false, "port not found");
          return x_root;
        }

        int ret = vnf->attach_port(pid, port);
        if (ret < 0) {
          crow::json::wvalue x_root;
          x_root["result"] = responce_info(false, "some error occured");
          return x_root;
        }
        crow::json::wvalue x_root;
        x_root["result"] = responce_info(true, "");
        return x_root;

      } else if (req.method == crow::HTTPMethod::DELETE) {

        int ret = vnf->dettach_port(pid);
        if (ret < 0) {
          crow::json::wvalue x_root;
          x_root["result"] = responce_info(false, "some error occured");
          return x_root;
        }
        crow::json::wvalue x_root;
        x_root["result"] = responce_info(true, "");
        return x_root;

      }

   } catch (std::exception& e) {
      printf("throwed: %s \n", e.what());
      crow::json::wvalue x_root;
      x_root["result"] = responce_info(false, e.what());
      return x_root;
   }
  });
}

void addroute__ports(ssn_nfvi& nfvi, crow::SimpleApp& app)
{
  CROW_ROUTE(app,"/ports")
  .methods("GET"_method)
  ([&nfvi](const crow::request& req) {

      assert(req.method == crow::HTTPMethod::GET);

      if (req.method == crow::HTTPMethod::GET) {

        crow::json::wvalue x;
        size_t n_port = nfvi.get_ports().size();
        x["result"] = responce_info(true, "");
        x["n_port"] = n_port;
        for (size_t i=0; i<n_port; i++) {
          const ssn_vnf_port* port = nfvi.get_ports().at(i);
          x[std::to_string(i)] = std::move(vnf_port_info(port));
        }
        return x;

      }
  });
}

void addroute__ports_NAME(ssn_nfvi& nfvi, crow::SimpleApp& app)
{
  CROW_ROUTE(app,"/ports/<str>")
  .methods("GET"_method, "POST"_method, "DELETE"_method)
  ([&nfvi](const crow::request& req, const std::string pname) {

      assert(  (req.method == crow::HTTPMethod::GET   )
           ||  (req.method == crow::HTTPMethod::POST  )
           ||  (req.method == crow::HTTPMethod::DELETE)  );

      if (req.method == crow::HTTPMethod::GET) {

        ssn_vnf_port* port = nfvi.find_port(pname.c_str());
        if (!port) {
          crow::json::wvalue x_root;
          x_root["result"] = responce_info(false, "port not found");
          return x_root;
        }

        crow::json::wvalue x_root;
        x_root["result"] = responce_info(true, "");
        x_root["port"] = vnf_port_info(port);
        return x_root;

      } else if (req.method == crow::HTTPMethod::POST) {

        printf("create port\n");
        if (nfvi.find_port(pname.c_str())) {
          crow::json::wvalue x_root;
          x_root["result"] = responce_info(false, "pname already exists");
          return x_root;
        }

        auto req_json = crow::json::load(req.body);
        const std::string cname   = req_json["cname"].s();

        if (cname == "tap") {

          /*
           * - cname
           * - options.ifname
           */
          std::string ifname = req_json["options"]["ifname"].s();
          rte_mempool* mp = nfvi.get_mp();
          ssn_portalloc_tap_arg arg = { mp, ifname };
          nfvi.port_alloc_from_catalog(cname.c_str(), pname.c_str(), &arg);

        } else if (cname == "pci") {

          /*
           * - cname
           * - options.pciaddr
           */
          std::string pciaddr = req_json["options"]["pciaddr"].s();
          rte_mempool* mp = nfvi.get_mp();
          ssn_portalloc_pci_arg arg = { mp, pciaddr };
          nfvi.port_alloc_from_catalog(cname.c_str(), pname.c_str(), &arg);

        } else if (cname == "virt") {

          /*
           * - cname
           */
          ssn_portalloc_virt_arg arg;
          nfvi.port_alloc_from_catalog(cname.c_str(), pname.c_str(), &arg);

        } else {

          crow::json::wvalue x_root;
          x_root["result"] = responce_info(false, "catalog name is invalid");
          return x_root;

        }

        ssn_vnf_port* port = nfvi.find_port(pname.c_str());
        port->config_hw(4,4);
        crow::json::wvalue x_root;
        x_root["result"] = responce_info(true, "");
        return x_root;

      } else if (req.method == crow::HTTPMethod::DELETE) {

        ssn_vnf_port* port = nfvi.find_port(pname.c_str());
        if (!port) {
          crow::json::wvalue x_root;
          x_root["result"] = responce_info(false, "port not found");
          return x_root;
        }

        if (port->is_attached_vnf()) {
          crow::json::wvalue x_root;
          x_root["result"] = responce_info(false, "port still attached to vnf");
          return x_root;
        }

        nfvi.del_port(port);
        crow::json::wvalue x_root;
        x_root["result"] = responce_info(true, "");
        return x_root;
      }
  });
}

void addroute__catalogs_vnf(ssn_nfvi& nfvi, crow::SimpleApp& app)
{
  CROW_ROUTE(app,"/catalogs/vnf")
  .methods("GET"_method)
  ([&nfvi](const crow::request& req) {

      assert(  (req.method == crow::HTTPMethod::GET   ) );

      if (req.method == crow::HTTPMethod::GET) {

        crow::json::wvalue x_root;
        const size_t n_ele = nfvi.get_vcat().size();
        x_root["result"] = responce_info(true, "");
        x_root["n_ele"]  = n_ele;

        crow::json::wvalue x_element;
        for (size_t i=0; i<n_ele; i++) {
          x_element["name"] = nfvi.get_vcat()[i].name;
          x_element["allocator"] = format("%p", nfvi.get_vcat()[i].allocator);
          x_root[std::to_string(i)] = std::move(x_element);
        }
        return x_root;

      }
  });
}

void addroute__catalogs_port(ssn_nfvi& nfvi, crow::SimpleApp& app)
{
  CROW_ROUTE(app,"/catalogs/port")
  .methods("GET"_method)
  ([&nfvi](const crow::request& req) {

      assert(  (req.method == crow::HTTPMethod::GET   ) );

      if (req.method == crow::HTTPMethod::GET) {

        crow::json::wvalue x_root;
        const size_t n_ele = nfvi.get_pcat().size();
        x_root["result"] = responce_info(true, "");
        x_root["n_ele"]  = n_ele;

        crow::json::wvalue x_element;
        for (size_t i=0; i<n_ele; i++) {
          x_element["name"] = nfvi.get_pcat()[i].name;
          x_element["allocator"] = format("%p", nfvi.get_pcat()[i].allocator);
          x_root[std::to_string(i)] = std::move(x_element);
        }
        return x_root;

      }
  });
}
#if 0

void addroute__vnf_STR_ports_INT(ssn_nfvi& nfvi, crow::SimpleApp& app)
{
  using std::string;
  using slankdev::format;

  CROW_ROUTE(app, "/vnfs/<str>/ports/<int>")
  .methods("PUT"_method, "DELETE"_method)
  ([&nfvi](const crow::request& req, std::string iname, int pid) {

    try {
      ssn_vnf* vnf = nfvi.find_vnf(iname.c_str());

      if (req.method == crow::HTTPMethod::PUT) {

        /*
         * {
         *    "piname" : "pci0"
         * }
         */

        auto req_json = crow::json::load(req.body);
        std::string piname = req_json["piname"].s();
        ssn_vnf_port* port = nfvi.find_port(piname.c_str());
        vnf->attach_port(pid, port);

      } else if (req.method == crow::HTTPMethod::DELETE) {

        /* no need options */
        vnf->dettach_port(pid);

      }

      crow::json::wvalue x_root;
      x_root["result"] = responce_info(true, "");
      return x_root;

    } catch (std::exception& e) {

      crow::json::wvalue x_root;
      x_root["result"] = responce_info(false, e.what());
      return x_root;

    }
  });
}

void addroute__vnfs_STR_coremask(ssn_nfvi& nfvi, crow::SimpleApp& app)
{
  using std::string;
  using slankdev::format;

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
}

void addroute__vnfs_STR_running(ssn_nfvi& nfvi, crow::SimpleApp& app)
{
  using std::string;
  using slankdev::format;

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

void addroute__portcatalog(ssn_nfvi& nfvi, crow::SimpleApp& app)
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
}

void addroute__ports_STR_config(ssn_nfvi& nfvi, crow::SimpleApp& app)
{
  using std::string;
  using slankdev::format;

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
#endif

} /* namespace */


int rest_api_thread(ssn_nfvi* nfviptr)
{
  using std::string;
  using slankdev::format;
  ssn_nfvi& nfvi = *nfviptr;

  crow::SimpleApp app;
  app.loglevel(crow::LogLevel::Critical);
  // app.loglevel(crow::LogLevel::Debug);

  addroute__                           (nfvi, app);
  addroute__vnfs                       (nfvi, app);
  addroute__vnfs_NAME                  (nfvi, app);
  addroute__vnfs_NAME_ports_PORTID     (nfvi, app);
  addroute__ports                      (nfvi, app);
  addroute__ports_NAME                 (nfvi, app);
  addroute__catalogs_vnf               (nfvi, app); // need test
  addroute__catalogs_port              (nfvi, app); // need test

#if 0
  addroute__vnfs_NAME_deploy           (nfvi, app); // not yet
  addroute__vnfs_NAME_undeploy         (nfvi, app); // not yet
  addroute__vnfs_NAME_coremask_BLOCKID (nfvi, app); // not yet
  addroute__vnfs_NAME_reset            (nfvi, app); // not yet
#endif

  app.port(8888).run();

}


