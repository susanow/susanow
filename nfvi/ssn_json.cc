
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

#include <ssn_nfvi.h>
#include <stdio.h>
#include <crow.h>
#include <string>
#include <slankdev/string.h>
#include <ssn_json.h>



crow::json::wvalue responce_info(bool success, const char* msg)
{
  crow::json::wvalue x;
  x["success"] = success;
  x["msg"] = msg;
  return x;
}

crow::json::wvalue core_info(const ssn_nfvi* nfvi, size_t lcore_id)
{
  crow::json::wvalue x_core;
  x_core["lcore_id"] = lcore_id;
  x_core["socket_id"] = rte_lcore_to_socket_id(lcore_id);
  x_core["state"] = ssn_lcore_state2str(ssn_get_lcore_state(lcore_id));
  if (lcore_id == rte_get_master_lcore()) x_core["state"] = "MASTER";
  x_core["usage_rate"] = nfvi->get_processor_rate(lcore_id);
  return x_core;
}

crow::json::wvalue cpu_info(const ssn_nfvi* nfvi, size_t socket_id)
{
  crow::json::wvalue x_cpu;
  x_cpu["socket_id"] = socket_id;
  const size_t n_cores = rte_lcore_count();
  x_cpu["n_core"] = n_cores / nfvi->n_socket();
  std::vector<size_t> lcore_ids;
  for (size_t i=0; i<n_cores; i++) {
    if (rte_lcore_to_socket_id(i) == socket_id)
      lcore_ids.push_back(i);
  }

  for (size_t i=0; i<lcore_ids.size(); i++) {
    size_t lcore_id = lcore_ids[i];
    x_cpu[std::to_string(i)] = core_info(nfvi, lcore_id);
  }

  return x_cpu;
}

crow::json::wvalue mempool_info(const rte_mempool* mp)
{
  crow::json::wvalue x_mempool;
  x_mempool["name"] = mp->name;
  x_mempool["size"] = mp->size;
  x_mempool["avail"] = rte_mempool_avail_count(mp);
  return x_mempool;
}

crow::json::wvalue nfvi_info(const ssn_nfvi* nfvi)
{
  using std::string;
  using slankdev::format;
  crow::json::wvalue x;

  x["n_socket"] = nfvi->n_socket();
  x["n_core"]   = nfvi->n_core();

  crow::json::wvalue x_cores;
  const size_t n_core = nfvi->n_core();
  for (size_t i=0; i<n_core; i++) {
    crow::json::wvalue x_core = core_info(nfvi, i);
    x_cores[std::to_string(i)] = std::move(x_core);
  }
  x["cores"] = std::move(x_cores);
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

  x["outer_rxp"] = port->get_outer_rx_perf();
  x["outer_txp"] = port->get_outer_tx_perf();
  x["inner_rxp"] = port->get_inner_rx_perf();
  x["inner_txp"] = port->get_inner_tx_perf();
  x["perfred" ] = port->get_perf_reduction();
  x["socket_id"] = port->get_socket_id();

  const ssn_vnf* vnf = port->get_attached_vnf();
  x["attach"  ] = vnf ? vnf->name : "nill";

  return x;
}

crow::json::wvalue pnic_info(size_t dpdk_port_id)
{
  crow::json::wvalue x;
  x["port_id"] = dpdk_port_id;
  x["tot_ipackets"] = ssn_port_stat_get_tot_rx_packets(dpdk_port_id);
  x["tot_opackets"] = ssn_port_stat_get_tot_tx_packets(dpdk_port_id);
  x["tot_ibytes"]   = ssn_port_stat_get_tot_rx_bytes(dpdk_port_id);
  x["tot_obytes"]   = ssn_port_stat_get_tot_tx_bytes(dpdk_port_id);

  x["cur_rx_pps"] = ssn_port_stat_get_cur_rx_pps(dpdk_port_id);
  x["cur_tx_pps"] = ssn_port_stat_get_cur_tx_pps(dpdk_port_id);
  x["cur_rx_bps"] = ssn_port_stat_get_cur_rx_bps(dpdk_port_id);
  x["cur_tx_bps"] = ssn_port_stat_get_cur_tx_bps(dpdk_port_id);
  x["cur_rx_mis"] = ssn_port_stat_get_cur_rx_mis(dpdk_port_id);
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
  x["socket_affinity"] = block->get_socket_affinity();
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



