
#pragma once

#include <stdio.h>
#include <slankdev/extra/dpdk.h>

rte_mempool* mp[RTE_MAX_ETHPORTS];

inline size_t ssn_dev_count() { return rte_eth_dev_count(); }
inline void ssn_port_init(size_t pid, size_t nb_rxq, size_t nb_txq)
{
  printf("%s(%zd, %zd, %zd) called\n", __func__, pid, nb_rxq, nb_txq);

}
inline void ssn_port_linkup(size_t p)
{
  // rte_eth_dev_link_up(p);
}

inline void ssn_port_init()
{
  size_t nb_ports = rte_eth_dev_count();
  printf("%zd ports found \n", nb_ports);
  for (size_t i=0; i<nb_ports; i++) {
    std::string name = slankdev::format("RXMP%zd", i);
    mp[i] = slankdev::mp_alloc(name.c_str());
    printf("mp alloc name=%s \n", name.c_str());
  }
}
inline void ssn_port_fin()
{
  size_t nb_ports = rte_eth_dev_count();
  for (size_t i=0; i<nb_ports; i++) {
    printf("mp free name=%s\n", mp[i]->name);
    rte_mempool_free(mp[i]);
  }
}


