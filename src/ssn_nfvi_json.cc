

#include <ssn_nfvi.h>
#include <stdio.h>
#include <crow.h>
#include <string>
#include <slankdev/string.h>
#include <ssn_nfvi_json.h>



crow::json::wvalue responce_info(bool success, const char* msg)
{
  crow::json::wvalue x;
  x["success"] = success;
  x["msg"] = msg;
  return x;
}


crow::json::wvalue vnf_port_info(const ssn_vnf_port* port)
{
  using std::string;
  using slankdev::format;
  crow::json::wvalue x;

  x["name"    ] = port->name;
  x["n_rxq"   ] = port->get_n_rxq();
  x["n_txq"   ] = port->get_n_txq();
  x["n_rxa"   ] = port->get_n_rxacc();
  x["n_txa"   ] = port->get_n_txacc();

#if 0 // TODO enable
  x["outer_rxp"] = port->get_outer_rx_perf();
  x["outer_txp"] = port->get_outer_tx_perf();
  x["inner_rxp"] = port->get_inner_rx_perf();
  x["inner_txp"] = port->get_inner_tx_perf();
#endif
  x["perfred" ] = port->get_perf_reduction();

  const ssn_vnf* vnf = port->get_attached_vnf();
  x["attach"  ] = vnf ? vnf->name : "nill";

  return x;
}


crow::json::wvalue vnf_block_info(const ssn_vnf_block* block)
{
  using std::string;
  using slankdev::format;

  crow::json::wvalue x;
  x["name"   ] = block->name;
  x["coremask"  ] = block->get_coremask();
  x["running"] = block->is_running();
  return x;
}


crow::json::wvalue vnf_info(const ssn_vnf* vnf)
{
  using std::string;
  using slankdev::format;

  crow::json::wvalue x;
  x["name"   ] = vnf->name;
  x["running"] = vnf->is_running();
  x["deployable"] = vnf->deployable();
  x["n_port" ] = vnf->n_ports();
  x["n_block"] = vnf->n_blocks();
  x["perfred"] = vnf->get_perf_reduction();
  x["rxrate" ] = vnf->get_rx_rate();

  crow::json::wvalue x_blocks;
  const size_t n_block = vnf->n_blocks();
  for (size_t i=0; i<n_block; i++) {
    x_blocks[std::to_string(i)] = vnf_block_info(vnf->get_block(i));
  }
  x["blocks"] = std::move(x_blocks);
  x["coremask" ] = vnf->get_coremask();

  crow::json::wvalue x_ports;
  const size_t n_port = vnf->n_ports();
  for (size_t i=0; i<n_port; i++) {
    const ssn_vnf_port* port =  vnf->get_port(i);
    if (port) x_ports[std::to_string(i)] = vnf_port_info(vnf->get_port(i));
    else      x_ports[std::to_string(i)] = "nil";
  }
  x["ports"] = std::move(x_ports);
  return x;
}

crow::json::wvalue ppp_info(const ssn_vnf_port_patch_panel* ppp)
{
  using std::string;
  using slankdev::format;

  crow::json::wvalue x;
  x["name"     ] = ppp->name;
  x["deletable"] = ppp->deletable();
  x["right"    ] = vnf_port_info(ppp->get_right());
  x["left"     ] = vnf_port_info(ppp->get_left() );
  return x;
}



