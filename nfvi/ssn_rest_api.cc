
#include <pch.h> // #include <crow.h>
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string>
#include <slankdev/string.h>
#include <slankdev/exception.h>
#include <slankdev/pci.h>
#include <ssn_nfvi.h>
#include <ssn_json.h>


static bool coremask_is_valid(size_t coremask)
{
  size_t n_core = rte_lcore_count();
  for (size_t i=0; i<n_core; i++) {
    if ((0x1<<i) & coremask != 0) {
      auto state = rte_eal_get_lcore_state(i);
      if (state != WAIT) {
        return false;
      }
    }
  }
  return true;
}


namespace {

using std::string;
using slankdev::format;


void addroute__(ssn_nfvi& nfvi, crow::App<Middleware>& app)
{
  CROW_ROUTE(app,"/")
  .methods("GET"_method)
  ([&nfvi]() {
      crow::json::wvalue x_root;
      x_root["result"] = responce_info(true, "give me much time... X(");
      x_root["running"] = true;
      x_root["uptime_sec"] = nfvi.get_uptime();
      return x_root;
  });
}

void addroute__system(ssn_nfvi& nfvi, crow::App<Middleware>& app)
{
  CROW_ROUTE(app,"/system")
  .methods("GET"_method)
  ([&nfvi]() {
      crow::json::wvalue x_root;
      x_root["result"] = responce_info(true, "");
      x_root["n_vnf"]  = nfvi.get_vnfs().size();
      x_root["n_port"] = nfvi.get_ports().size();
      x_root["n_vcat"] = nfvi.get_vcat().size();

      x_root["nfvi"] = nfvi_info(&nfvi);
      return x_root;
  });
}

void addroute__system_cpu(ssn_nfvi& nfvi, crow::App<Middleware>& app)
{
  CROW_ROUTE(app,"/system/cpu")
  .methods("GET"_method)
  ([&nfvi]() {
      crow::json::wvalue x_root;
      x_root["result"] = responce_info(true, "");
      size_t n_socket = nfvi.n_socket();
      x_root["n_cpu"] = n_socket;
      for (size_t i=0; i<n_socket; i++) {
        x_root[std::to_string(i)] = cpu_info(&nfvi, i);
      }
      return x_root;
  });
}

void addroute__system_mem(ssn_nfvi& nfvi, crow::App<Middleware>& app)
{
  CROW_ROUTE(app,"/system/mem")
  .methods("GET"_method)
  ([&nfvi]() {
      crow::json::wvalue x_root;
      x_root["result"] = responce_info(true, "");

      size_t n_mempool = nfvi.n_socket();
      x_root["n_mempool"] = n_mempool;
      for (size_t i=0; i<n_mempool; i++) {
        x_root[std::to_string(i)] = mempool_info(nfvi.get_mp(i));
      }
      return x_root;
  });
}

void addroute__system_pnic(ssn_nfvi& nfvi, crow::App<Middleware>& app)
{
  CROW_ROUTE(app,"/system/pnic")
  .methods("GET"_method)
  ([&nfvi]() {
      crow::json::wvalue x_root;
      x_root["result"] = responce_info(true, "");

      size_t n_pnic = rte_eth_dev_count();
      x_root["n_pnic"] = n_pnic;
      for (size_t i=0; i<n_pnic; i++) {
        x_root[std::to_string(i)] = pnic_info(i);
      }
      return x_root;
  });
}

void addroute__vnfs(ssn_nfvi& nfvi, crow::App<Middleware>& app)
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

void addroute__vnfs_NAME(ssn_nfvi& nfvi, crow::App<Middleware>& app)
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

void addroute__vnfs_NAME_ports_PORTID(ssn_nfvi& nfvi, crow::App<Middleware>& app)
{
  CROW_ROUTE(app, "/vnfs/<str>/ports/<int>")
  .methods("PUT"_method, "DELETE"_method)
  ([&nfvi](const crow::request& req, std::string iname, int pid) {

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

        if (vnf->is_running()) {
          crow::json::wvalue x_root;
          x_root["result"] = responce_info(false, "vnf is still running");
          return x_root;
        }

        auto req_json = crow::json::load(req.body);
        std::string pname = req_json["pname"].s();
        ssn_vnf_port* port = nfvi.find_port(pname.c_str());
        if (!port) {
          crow::json::wvalue x_root;
          x_root["result"] = responce_info(false, "port not found");
          return x_root;
        }

        if (port->is_attached_vnf()) {
          crow::json::wvalue x_root;
          x_root["result"] = responce_info(false, "port has attached vnf");
          return x_root;
        }

        int ret = vnf->attach_port(pid, port);
        if (ret < 0) {
          crow::json::wvalue x_root;
          x_root["result"] = responce_info(false, "vnf::attach_port returned -1");
          return x_root;
        }
        crow::json::wvalue x_root;
        x_root["result"] = responce_info(true, "");
        return x_root;

      } else if (req.method == crow::HTTPMethod::DELETE) {

        if (vnf->is_running()) {
          crow::json::wvalue x_root;
          x_root["result"] = responce_info(false, "vnf is still running");
          return x_root;
        }

        int ret = vnf->dettach_port(pid);
        if (ret < 0) {
          crow::json::wvalue x_root;
          x_root["result"] = responce_info(false, "vnf::dettach_port returned -1");
          return x_root;
        }
        crow::json::wvalue x_root;
        x_root["result"] = responce_info(true, "");
        return x_root;

      }
  });
}

void addroute__ports(ssn_nfvi& nfvi, crow::App<Middleware>& app)
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

void addroute__ports_NAME(ssn_nfvi& nfvi, crow::App<Middleware>& app)
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

        if (nfvi.find_port(pname.c_str())) {
          crow::json::wvalue x_root;
          x_root["result"] = responce_info(false, "pname already exists");
          return x_root;
        }

        auto req_json = crow::json::load(req.body);
        const std::string cname   = req_json["cname"].s();

        try {
          if (cname == "tap") {

            /*
             * - cname
             * - options.ifname
             */
            std::string ifname = req_json["options"]["ifname"].s();
            ssn_vnf_port_dpdk* port = new ssn_vnf_port_dpdk_tap(pname.c_str(), ifname.c_str());
            port->set_mp(nfvi.get_mp(0)); // TODO support NUMA-Aware
            nfvi.append_port(port);

          } else if (cname == "pipe") {

            /*
             * - cname
             * - options.pciaddr
             */
            printf("SLANKDEV create pipe\n");
            assert(nfvi.nrxq == nfvi.ntxq);
            ssn_vnf_port_dpdk* port = new ssn_vnf_port_dpdk_pipe(pname.c_str());
            port->set_mp(nfvi.get_mp(0)); // TODO support NUMA-Aware
            port->config_hw(nfvi.nrxq,nfvi.ntxq);
            nfvi.append_port(port);

          } else if (cname == "pci") {

            /*
             * - cname
             * - options.pciaddr
             */
            std::string pciaddr = req_json["options"]["pciaddr"].s();
            ssn_vnf_port_dpdk* port = new ssn_vnf_port_dpdk_pci(pname.c_str(), pciaddr.c_str());
            size_t socket_id = slankdev::get_numa_node(pciaddr.c_str());
            port->set_mp(nfvi.get_mp(socket_id));
            port->config_hw(nfvi.nrxq,nfvi.ntxq);
            nfvi.append_port(port);

          } else if (cname == "vhost") {

            /*
             * - cname
             * - options.pciaddr
             */
            printf("SLANKDEV create vhost\n");
            assert(nfvi.nrxq == nfvi.ntxq);
            ssn_vnf_port_dpdk* port = new ssn_vnf_port_dpdk_vhost(pname.c_str(), nfvi.nrxq);
            port->set_mp(nfvi.get_mp(0)); // TODO support NUMA-Aware
            port->config_hw(nfvi.nrxq,nfvi.ntxq);
            nfvi.append_port(port);

          } else if (cname == "afpacket") {

            /*
             * - cname
             * - options.ifname
             */
            std::string ifname = req_json["options"]["ifname"].s();
            assert(nfvi.nrxq == nfvi.ntxq);
            ssn_vnf_port_dpdk* port = new ssn_vnf_port_dpdk_afpacket(
                    pname.c_str(), ifname.c_str(), nfvi.nrxq);
            port->set_mp(nfvi.get_mp(0)); // TODO support NUMA-Aware
            port->config_hw(nfvi.nrxq,nfvi.ntxq);
            nfvi.append_port(port);

          } else if (cname == "virt") {

            /*
             * - cname
             */
            ssn_vnf_port* port = new ssn_vnf_port_virt(pname.c_str());
            port->config_hw(nfvi.nrxq,nfvi.ntxq);
            nfvi.append_port(port);

          } else {

            crow::json::wvalue x_root;
            x_root["result"] = responce_info(false, "catalog name is invalid");
            return x_root;

          }

        } catch (std::exception& e) {

          crow::json::wvalue x_root;
          std::string err = slankdev::format("%s", e.what());
          x_root["result"] = responce_info(false, err.c_str());
          return x_root;

        }

        ssn_vnf_port* port = nfvi.find_port(pname.c_str());
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

void addroute__catalogs_vnf(ssn_nfvi& nfvi, crow::App<Middleware>& app)
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

void addroute__vnfs_NAME_coremask_BLOCKID(ssn_nfvi& nfvi, crow::App<Middleware>& app)
{
  CROW_ROUTE(app, "/vnfs/<str>/coremask/<int>") .methods("PUT"_method)
  ([&nfvi](const crow::request& req, std::string str, int blockId) {

    assert(req.method == crow::HTTPMethod::PUT);

    if (req.method == crow::HTTPMethod::PUT) {

      /*
       * Find VNF by name...
       */
      auto* vnf = nfvi.find_vnf(str.c_str());
      if (!vnf) {
        crow::json::wvalue x;
        x["result"] = responce_info(false, "not found vnf");
        return x;
      }

      /*
       * Check VNF State. if state is running then, return with err.
       */
      if (vnf->is_running()) {
        crow::json::wvalue x;
        x["result"] = responce_info(false, "vnf still running");
        return x;
      }

      /*
       * Check VNF access-infos were already reset.
       * If unreset that, return error-msg
       */
      if (vnf->get_coremask(blockId) != 0) {
        crow::json::wvalue x;
        x["result"] = responce_info(false, "vnf is not reset yet");
        return x;
      }

      auto req_json = crow::json::load(req.body);
      const uint32_t coremask = req_json["coremask"].i();

      /*
       * Check coremask is valid
       */
      if (!coremask_is_valid(coremask)) {
        crow::json::wvalue x;
        x["result"] = responce_info(false, "coremask is invalid");
        return x;
      }

      /*
       * Set coremask
       */
      int ret = vnf->set_coremask(blockId, coremask);
      if (ret < 0) {
        crow::json::wvalue x;
        x["result"] = responce_info(false, "vnf can't set coremask");
        return x;
      }

      crow::json::wvalue x;
      std::string msg = format(
          "%s: coremask bid=%d mask=0x%x",
          vnf->name.c_str(), blockId, coremask);
      x["result"] = responce_info(true, msg.c_str());
      return x;
    }
  });
}

void addroute__vnfs_NAME_reset(ssn_nfvi& nfvi, crow::App<Middleware>& app)
{
  CROW_ROUTE(app, "/vnfs/<str>/reset")
  .methods("PUT"_method)
  ([&nfvi](const crow::request& req, std::string str) {
    assert(req.method == crow::HTTPMethod::PUT);

    if (req.method == crow::HTTPMethod::PUT) {

      /*
       * Find VNF by name...
       */
      auto* vnf = nfvi.find_vnf(str.c_str());
      if (!vnf) {
        crow::json::wvalue x;
        x["result"] = responce_info(false, "not found vnf");
        return x;
      }

      /*
       * Check VNF State. if state is running then, return with err.
       */
      if (vnf->is_running()) {
        crow::json::wvalue x;
        x["result"] = responce_info(false, "vnf still running");
        return x;
      }

      /*
       * Reset port accessor
       */
      int ret = vnf->reset();
      if (ret < 0) {
        crow::json::wvalue x;
        x["result"] = responce_info(false, "vnf can't reset port-access-config");
        return x;
      }

      crow::json::wvalue x;
      x["result"] = responce_info(true, "reset vnf's running config");
      return x;
    }
  });
}

void addroute__vnfs_NAME_deploy(ssn_nfvi& nfvi, crow::App<Middleware>& app)
{

  CROW_ROUTE(app, "/vnfs/<str>/deploy")
  .methods("PUT"_method)
  ([&nfvi](const crow::request& req, std::string str) {

    assert(req.method == crow::HTTPMethod::PUT);

    if (req.method == crow::HTTPMethod::PUT) {

      /*
       * Find VNF by name...
       */
      auto* vnf = nfvi.find_vnf(str.c_str());
      if (!vnf) {
        crow::json::wvalue x;
        x["result"] = responce_info(false, "not found vnf");
        return x;
      }

      /*
       * Check VNF State. if state is running then, return with err.
       */
      if (vnf->is_running()) {
        crow::json::wvalue x;
        x["result"] = responce_info(false, "already running");
        return x;
      }

      int ret = vnf->deploy();
      if (ret < 0) {
        crow::json::wvalue x;
        x["result"] = responce_info(false, "vnf could't deploy");
        return x;
      }

      crow::json::wvalue x;
      x["result"] = responce_info(true, "deploy successful");
      return x;
    }
  });
}

void addroute__vnfs_NAME_undeploy(ssn_nfvi& nfvi, crow::App<Middleware>& app)
{
  CROW_ROUTE(app, "/vnfs/<str>/undeploy")
  .methods("PUT"_method)
  ([&nfvi](const crow::request& req, std::string str) {

    assert(req.method == crow::HTTPMethod::PUT);

    if (req.method == crow::HTTPMethod::PUT) {

      /*
       * Find VNF by name...
       */
      auto* vnf = nfvi.find_vnf(str.c_str());
      if (!vnf) {
        crow::json::wvalue x;
        x["result"] = responce_info(false, "not found vnf");
        return x;
      }

      /*
       * Check VNF is running
       */
      if (!vnf->is_running()) {
        crow::json::wvalue x;
        x["result"] = responce_info(false, "vnf is not running");
        return x;
      }

      vnf->undeploy();
      crow::json::wvalue x;
      x["result"] = responce_info(true, "undeploy successful");
      return x;
    }
  });
}

void addroute__ppps(ssn_nfvi& nfvi, crow::App<Middleware>& app)
{
  CROW_ROUTE(app,"/ppps")
  .methods("GET"_method)
  ([&nfvi](const crow::request& req) {

      assert(  (req.method == crow::HTTPMethod::GET   )  );

      if (req.method == crow::HTTPMethod::GET) {

        auto ppps = nfvi.get_ppps();
        crow::json::wvalue x_root;
        x_root["result"] = responce_info(true, "");
        const size_t n_ele = ppps.size();
        x_root["n_ele"] = n_ele;
        for (size_t i=0; i<n_ele; i++) {
          x_root[std::to_string(i)] = ppp_info(ppps[i]);
        }
        return x_root;

      }
  });
}

void addroute__ppps_NAME(ssn_nfvi& nfvi, crow::App<Middleware>& app)
{
  CROW_ROUTE(app,"/ppps/<str>")
  .methods("GET"_method, "POST"_method, "DELETE"_method)
  ([&nfvi](const crow::request& req, const std::string name) {

      assert(  (req.method == crow::HTTPMethod::GET   )
           ||  (req.method == crow::HTTPMethod::POST  )
           ||  (req.method == crow::HTTPMethod::DELETE)  );

      if (req.method == crow::HTTPMethod::GET) {

        ssn_vnf_port_patch_panel* ppp = nfvi.find_ppp(name.c_str());
        if (!ppp) {
          crow::json::wvalue x_root;
          x_root["result"] = responce_info(false, "ppp not found");
          return x_root;
        }

        crow::json::wvalue x_root;
        x_root["result"] = responce_info(true, "");
        x_root["ppp"] = ppp_info(ppp);
        return x_root;

      } else if (req.method == crow::HTTPMethod::POST) {

        if (nfvi.find_ppp(name.c_str())) {
          crow::json::wvalue x_root;
          x_root["result"] = responce_info(false, "ppp already exists");
          return x_root;
        }

        auto req_json = crow::json::load(req.body);
        const std::string right_name = req_json["right"].s();
        const std::string left_name  = req_json["left" ].s();

        ssn_vnf_port* right_port = nfvi.find_port(right_name.c_str());
        ssn_vnf_port* left_port  = nfvi.find_port(left_name.c_str() );
        ssn_vnf_port_patch_panel* ppp = nfvi.ppp_alloc(name.c_str(), right_port, left_port);
        if (!ppp) {
          crow::json::wvalue x_root;
          x_root["result"] = responce_info(false, "unsuccess to allocate ppp");
          return x_root;
        }

        crow::json::wvalue x_root;
        x_root["result"] = responce_info(true, "");
        return x_root;

      } else if (req.method == crow::HTTPMethod::DELETE) {

        ssn_vnf_port_patch_panel* ppp = nfvi.find_ppp(name.c_str());
        if (!ppp) {
          crow::json::wvalue x_root;
          x_root["result"] = responce_info(false, "ppp not found");
          return x_root;
        }

        if (!ppp->deletable()) {
          crow::json::wvalue x_root;
          x_root["result"] = responce_info(false, "ppp is not deletable");
          return x_root;
        }

        nfvi.del_ppp(ppp);
        crow::json::wvalue x_root;
        x_root["result"] = responce_info(true, "");
        return x_root;
      }
  });
}

} /* namespace */



void rest_api_thread(ssn_nfvi* nfviptr,
    crow::App<Middleware>* app,
    uint16_t rest_server_port)
{
  using std::string;
  using slankdev::format;
  ssn_nfvi& nfvi = *nfviptr;
  app->loglevel(crow::LogLevel::Critical);
  app->loglevel(crow::LogLevel::Debug);
  app->loglevel(crow::LogLevel::Error);

  addroute__                             (nfvi, *app);
  addroute__system                       (nfvi, *app);
  addroute__system_cpu                   (nfvi, *app);
  addroute__system_mem                   (nfvi, *app);
  addroute__system_pnic                  (nfvi, *app);
  addroute__vnfs                         (nfvi, *app);
  addroute__vnfs_NAME                    (nfvi, *app);
  addroute__vnfs_NAME_ports_PORTID       (nfvi, *app);
  addroute__ports                        (nfvi, *app);
  addroute__ports_NAME                   (nfvi, *app);
  addroute__catalogs_vnf                 (nfvi, *app);
  addroute__vnfs_NAME_coremask_BLOCKID   (nfvi, *app);
  addroute__vnfs_NAME_reset              (nfvi, *app);
  addroute__vnfs_NAME_deploy             (nfvi, *app);
  addroute__vnfs_NAME_undeploy           (nfvi, *app);
  addroute__ppps                         (nfvi, *app);
  addroute__ppps_NAME                    (nfvi, *app);

  app->port(rest_server_port).run();
}


