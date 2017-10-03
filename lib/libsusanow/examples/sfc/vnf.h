
#pragma once
#include <ssn_port.h>
#include <ssn_common.h>
#include <ssn_log.h>
#include <dpdk/dpdk.h>
#include <ssn_vnf_v02.h>

inline size_t get_oportid_from_iportid(size_t in_port_id) { return in_port_id^1; }

class vnf_block : public ssn_vnf_block {
  bool running = false;
  const std::string name;
 public:
  vnf_block(fixed_size_vector<ssn_vnf_port*>& ports, const char* n) : ssn_vnf_block(ports), name(n) {}
  virtual bool is_running() const override { return running; }
  virtual void undeploy_impl() override { running = false; }
  virtual void debug_dump(FILE* fp) const override { throw slankdev::exception("NOTIMPL"); }
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
    size_t lcore_id = ssn_lcore_id();
    size_t vcore_id  = get_vlcore_id();

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
          printf("%s recv port=%zd \n", name.c_str(), pid);

          /* Delay Block begin */
          size_t n=10;
          for (size_t j=0; j<100; j++) n++;

          tx_burst(pid^1, txaid, &mbufs[i], 1);
        }
      } //for
    } /* while (running) */
  }
};

class vnf : public ssn_vnf {
 public:
  vnf(const char* name) : ssn_vnf(2)
  {
    ssn_vnf_block* block = new vnf_block(ports, name);
    blocks.push_back(block);
  }
  ~vnf()
  {
    auto* p = blocks.at(blocks.size()-1);
    delete p;
    blocks.pop_back();
  }
}; /* class vnf */
