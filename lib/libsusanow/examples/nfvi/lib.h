
#pragma once

#include <slankdev/string.h>
#include <dpdk/dpdk.h>

namespace dpdk {
  size_t eth_dev_attach(const char* devargs)
  {
    uint8_t new_pid;
    int ret = rte_eth_dev_attach(devargs, &new_pid);
    if (ret < 0) {
      std::string err = slankdev::format("dpdk::eth_dev_attach (ret=%d)", ret);
      throw dpdk::exception(err.c_str());
    }
    return new_pid;
  }
  void eth_dev_detach(size_t port_id)
  {
    rte_eth_dev_stop(port_id);
    rte_eth_dev_close(port_id);
    char devname[1000];
    int ret = rte_eth_dev_detach(port_id, devname);
    if (ret < 0) {
      std::string err = slankdev::format("dpdk::eth_dev_detach (ret=%d)", ret);
      throw dpdk::exception(err.c_str());
    }
    RTE_LOG(INFO, USER1, "Ethernet device \'%s\' was detached by ssn_nfvi\n", devname);
  }
} // namespace dpdk
