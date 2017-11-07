
#pragma once
#include <stdint.h>
#include <stddef.h>



class ssn_vnf_port {
 public:
  const std::string name;
  const size_t n_rxq;
  const size_t n_txq;
  const size_t n_rxacc;
  const size_t n_txacc;
  const double perf_reduction;
  ssn_vnf_port(const char* n,
      size_t nrxq, size_t ntxq,
      size_t nrxa, size_t ntxa, double pr)
    : name(n)
    , n_rxq(nrxq)
    , n_txq(ntxq)
    , n_rxacc(nrxa)
    , n_txacc(ntxa)
    , perf_reduction(pr) {}
};

class ssn_vnf_block {
 public:
  const size_t n_vcores;
  ssn_vnf_block(size_t n_v) :n_vcores(n_v) {}
};

class ssn_vnf {
 public:
  const std::string name;
  uint64_t coremask;
  bool running;
  std::vector<ssn_vnf_block> blocks;
  std::vector<ssn_vnf_port*>  ports;
  ssn_vnf(const char* n, size_t n_block, bool r,
      ssn_vnf_port* port0, ssn_vnf_port* port1)
    : name(n), coremask(0x0), running(r), ports(2)
  {
    for (size_t i=0; i<n_block; i++) {
      blocks.emplace_back(1);
    }
    ports[0] = port0;
    ports[1] = port1;
  }
};

class ssn_nfvi {
 public:
  std::vector<ssn_vnf> vnfs;
  std::vector<ssn_vnf_port> ports;
  ssn_vnf_port* find_port(const char* name)
  {
    size_t n_port = ports.size();
    for (size_t i=0; i<n_port; i++) {
      if (ports[i].name == name) return &ports[i];
    }
    return nullptr;
  }
  ssn_vnf* find_vnf(const char* name)
  {
    size_t n_vnf = vnfs.size();
    for (size_t i=0; i<n_vnf; i++) {
      if (vnfs[i].name == name) return &vnfs[i];
    }
    return nullptr;
  }

  ssn_nfvi()
  {
    ports.emplace_back("dpdk0", 4, 4, 2, 2, 0.8);
    ports.emplace_back("dpdk1", 4, 4, 2, 2, 0.8);
    ports.emplace_back("dpdk2", 4, 4, 2, 2, 0.8);
    ports.emplace_back("dpdk3", 4, 4, 2, 2, 0.8);
    ports.emplace_back("dpdk4", 4, 4, 2, 2, 0.8);
    ports.emplace_back("dpdk5", 4, 4, 2, 2, 0.8);

    vnfs.emplace_back("vnf0", 1, false, &ports[0], &ports[1]);
    vnfs.emplace_back("vnf1", 1, false, &ports[2], &ports[3]);
    vnfs.emplace_back("vnf2", 1, false, &ports[4], &ports[5]);
  }
};



