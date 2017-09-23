
#pragma once
#include <stdio.h>
#include <stddef.h>
#include <stdint.h>
#include <vector>
#include <utility>
#include <slankdev/exception.h>

#include <ssn_port.h>
#include <ssn_log.h>
#include <ssn_cpu.h>
#include <ssn_common.h>
#include <ssn_thread.h>
#include <ssn_port_stat.h>


void ssn_vnf_port_debug_dump(FILE* fp);

void ssn_vnf_port_link_up(size_t pid);
void ssn_vnf_port_link_down(size_t pid);
void ssn_vnf_port_promisc_on(size_t pid);
void ssn_vnf_port_promisc_off(size_t pid);
void ssn_vnf_port_dev_up(size_t pid);
void ssn_vnf_port_dev_down(size_t pid);

void ssn_vnf_port_configure_hw(size_t dpdk_port_id, size_t n_rxq, size_t n_txq);
void ssn_vnf_port_configure_acc(size_t dpdk_port_id, size_t n_rxacc, size_t n_txacc);

size_t ssn_vnf_port_rx_burst(size_t port_id, size_t aid, rte_mbuf** mbufs, size_t n_mbufs);
size_t ssn_vnf_port_tx_burst(size_t port_id, size_t aid, rte_mbuf** mbufs, size_t n_mbufs);


