
#pragma once

#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <vector>
#include <string>
#include <ssn_sys.h>
#include <ssn_log.h>

ssize_t get_free_lcore_id()
{
  size_t nb_lcores = ssn_lcore_count();
  for (size_t i=1; i<nb_lcores; i++) {
    auto s = ssn_get_lcore_state(i);
    if (s == SSN_LS_WAIT) return i;
  }
  return -1;
}

class pipeline_stage {
 public:
  ssn_function_t f;
  void* arg;
  size_t mux;
  pipeline_stage(ssn_function_t ff, void* a) : f(ff), arg(a), mux(0) {}
  void inc()
  {
    ssize_t lcore_id = get_free_lcore_id();
    if (lcore_id < 0) {
      ssn_log(SSN_LOG_INFO, "could'nt increment therad\n");
      return ;
    }
    ssn_native_thread_launch(f, arg, lcore_id);
    ssn_log(SSN_LOG_INFO, "increment therad\n");
    mux++;
  }
};

class vnf {
 public:
  std::vector<pipeline_stage> pl;
  std::string name;
  vnf(const char* n) : name(n) {}
  virtual ~vnf() {}
  void debug_dump(FILE* fp)
  {
    size_t nb_pl = pl.size();
    for (size_t i=0; i<nb_pl; i++) {
      fprintf(fp ," pipeline[%zd]: f=%p, arg=%p mux=%zd\r\n",
          i, pl[i].f, pl[i].arg, pl[i].mux);
    }
  }
};
