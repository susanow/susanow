
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

#pragma once
#include <ssn_common.h>
#include <ssn_log.h>
#include <dpdk/dpdk.h>
#include <ssn_vnf.h>
#include <slankdev/socketfd.h>
#include <slankdev/net/addr.h>
#include <slankdev/net/hdr.h>

struct ipv4_l3fwd_lpm_route {
	uint32_t ip;
	uint8_t  depth;
	uint8_t  if_out;
};

const size_t ipv4_n_routes = 4;
static struct ipv4_l3fwd_lpm_route ipv4_routes[] = {
    {IPv4(192, 168, 10, 0), 24, 0},
    {IPv4(192, 168, 20, 0), 24, 1},
    {IPv4(1  , 1  , 1 , 1), 32, 1},
    {IPv4(2  , 2  , 2 , 2), 32, 0},
};


struct rte_lpm* setup_lpm(const int socketid,
    struct ipv4_l3fwd_lpm_route* routes, size_t n_routes);

class ssn_vnf_l3fwd_block : public ssn_vnf_block {

  size_t lpm_get_ipv4_dport(void *ipv4_hdr_,
      uint8_t portid, struct rte_lpm* lookup_struct)
  {
#if 1
    const struct ipv4_hdr* ih = (struct ipv4_hdr*)ipv4_hdr_;
    for (size_t i=0; i<ipv4_n_routes; i++) {
      ipv4_l3fwd_lpm_route& route = ipv4_routes[i];
      uint32_t tmp = rte_be_to_cpu_32(ih->dst_addr);
      uint32_t mask = 0x0;
      for (size_t j=0; j<route.depth; j++) {
        mask += (0x80000000 >> j);
      }
      tmp &= mask;
      if (tmp == (route.ip&mask)) {
        // printf("Found\n");
        return route.if_out;
      }
    }
    printf("NoFound\n");
    return portid;
#else
    uint32_t next_hop;
    struct ipv4_hdr* ih = (struct ipv4_hdr*)ipv4_hdr_;

    size_t ret = rte_lpm_lookup(lookup_struct, rte_be_to_cpu_32(ih->dst_addr), &next_hop);

    printf("ret: %zd  ", ret);
    if (ret == -EINVAL)      printf("EINVAL\n");
    else if (ret == -ENOENT) printf("ENOENT\n");
    else                     printf("SUCC? \n");

    ret = (ret == 0 ? next_hop : portid);
    return ret;
#endif
  }


  bool running = false;
  std::vector<struct rte_lpm*>& lpm_struct;
 public:
  ssn_vnf_l3fwd_block(
      slankdev::fixed_size_vector<ssn_vnf_port*>& ports, const char* n,
      std::vector<struct rte_lpm*> lpm_struct_)
    : ssn_vnf_block(ports, n) , lpm_struct(lpm_struct_) {}

  virtual bool is_running() const override { return running; }
  virtual void undeploy_impl() override { running = false; }
  virtual void debug_dump(FILE* fp) const override { fprintf(fp, "non\r\n"); }
  virtual void set_coremask_impl(uint32_t coremask) override
  {
    size_t n_vcores = slankdev::popcnt32(coremask);
    for (size_t i=0; i<n_vcores; i++) {
      size_t n_port = n_ports();
      for (size_t pid=0; pid<n_port; pid++) {
        size_t rxaid = port_request_rx_access(pid);
        set_lcore_port_rxaid(i, pid, rxaid);
        size_t txaid = port_request_tx_access(pid);
        set_lcore_port_txaid(i, pid, txaid);
      }
    }
  }
  virtual void deploy_impl(void*) override
  {
    const size_t lcore_id = dpdk::lcore_id();
    const size_t vcore_id  = get_vlcore_id();
    const size_t sockid = rte_socket_id();

    running = true;
    while (running) {
      size_t n_port = this->n_ports();
      for (size_t pid=0; pid<n_port; pid++) {

        rte_mbuf* mbufs[32];
        size_t rxaid = get_lcore_port_rxaid(vcore_id, pid);
        size_t txaid = get_lcore_port_txaid(vcore_id, pid^1);

        size_t n_recv = rx_burst(pid, rxaid, mbufs, 32);
        if (n_recv == 0) continue;

        for (size_t i=0; i<n_recv; i++) {

          rte_mbuf* m = mbufs[i];
          ether_hdr* eth_hdr = rte_pktmbuf_mtod(m, struct ether_hdr *);
          if (!RTE_ETH_IS_IPV4_HDR(m->packet_type)) rte_pktmbuf_free(m);

          struct ipv4_hdr* ih = (struct ipv4_hdr*)(eth_hdr + 1);
          size_t dstport = lpm_get_ipv4_dport(ih, pid, lpm_struct[sockid]);
          printf("inport%zd %s->%s  [dstport:%zd] \n", pid,
              slankdev::inaddr2str(ih->src_addr).c_str(),
              slankdev::inaddr2str(ih->dst_addr).c_str(), dstport);
          if (dstport > 1) dstport = pid;

          for (size_t k=0; k<6; k++) {
            eth_hdr->s_addr.addr_bytes[k] = k;
            eth_hdr->d_addr.addr_bytes[k] = 0xff;
          }

          size_t n_send = tx_burst(dstport, txaid, &mbufs[i], 1);
          if (n_send < 1) rte_pktmbuf_free(mbufs[i]);
        }

      } /* for */
    } /* while (running) */
  }

}; /* class ssn_vnf_l3fwd_block */


class ssn_vnf_l3fwd : public ssn_vnf {
  std::vector<struct rte_lpm*> lpm_struct;
 public:

  ssn_vnf_l3fwd(const char* name) : ssn_vnf(2, name)
  {
    lpm_struct.resize(2);
    lpm_struct[0] = setup_lpm(0, ipv4_routes, ipv4_n_routes);
    lpm_struct[1] = setup_lpm(1, ipv4_routes, ipv4_n_routes);

    std::string bname = name;
    bname += "block0";
    ssn_vnf_block* block = new ssn_vnf_l3fwd_block(ports, bname.c_str(), lpm_struct);
    blocks.push_back(block);
  }
  ~ssn_vnf_l3fwd()
  {
    auto* p = blocks.at(blocks.size()-1);
    delete p;
    blocks.pop_back();
  }
}; /* class ssn_vnf_l3fwd1b */

inline ssn_vnf*
ssn_vnfalloc_l3fwd(const char* instance_name)
{ return new ssn_vnf_l3fwd(instance_name); }


inline struct rte_lpm*
setup_lpm(const int socketid, struct ipv4_l3fwd_lpm_route* routes, size_t n_routes)
{
  printf("%s(%d, %p, %zd)\n", __func__, socketid, routes, n_routes);
	char s[64];
	snprintf(s, sizeof(s), "IPV4_L3FWD_LPM_%d", socketid);

	/* create the LPM table */
	struct rte_lpm_config config_ipv4;
  constexpr size_t IPV4_L3FWD_LPM_MAX_RULES    = 1024;
  constexpr size_t IPV4_L3FWD_LPM_NUMBER_TBL8S = (1<<8);
	config_ipv4.max_rules = IPV4_L3FWD_LPM_MAX_RULES;
	config_ipv4.number_tbl8s = IPV4_L3FWD_LPM_NUMBER_TBL8S;
	config_ipv4.flags = 0;

	struct rte_lpm* lpm_struct = rte_lpm_create(s, socketid, &config_ipv4);
	if (lpm_struct == NULL)
		rte_exit(EXIT_FAILURE,
			"Unable to create the l3fwd LPM table on socket %d\n",
			socketid);

	for (size_t i = 0; i <n_routes ; i++) {

		/* skip unused ports */
		if ((1 << routes[i].if_out) == 0)
			continue;

    int ret = rte_lpm_add( lpm_struct,
			routes[i].ip, routes[i].depth, routes[i].if_out);
		if (ret < 0) {
			rte_exit(EXIT_FAILURE,
				"Unable to add entry %zd to the l3fwd LPM table on socket %d\n",
				i, socketid);
		}

		printf("LPM: Adding route 0x%08x / %d (%d)\n",
			(unsigned)routes[i].ip, routes[i].depth, routes[i].if_out);
	}
  return lpm_struct;
}


