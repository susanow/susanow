
#pragma once
#include <stdint.h>
#include <stddef.h>

void ssn_port_stat_update(void*);
void ssn_port_stat_init();
void ssn_port_stat_fin();

size_t ssn_port_stat_get_cur_rx_pps(size_t pid);
size_t ssn_port_stat_get_cur_tx_pps(size_t pid);
size_t ssn_port_stat_get_cur_rx_bps(size_t pid);
size_t ssn_port_stat_get_cur_tx_bps(size_t pid);

size_t ssn_port_stat_get_tot_rx_packets(size_t pid);
size_t ssn_port_stat_get_tot_tx_packets(size_t pid);
size_t ssn_port_stat_get_tot_rx_bytes(size_t pid);
size_t ssn_port_stat_get_tot_tx_bytes(size_t pid);


