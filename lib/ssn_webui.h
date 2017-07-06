
#pragma once
#include <string>
#include <vector>

class ssn_stats_vnf {
 public:
  int id;
  std::string name;
  int stages;

  void dump(FILE* fp) const;
};

class ssn_stats {
 public:
  int nb_threads;
  int nb_cpus;
  int nb_ports;
  std::string author;
  std::vector<ssn_stats_vnf> vnfs;

  void dump(FILE* fp) const;
};



