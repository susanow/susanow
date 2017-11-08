
#include <vnf.h>

#include <string>
#include <vector>
#include <stdio.h>

#include <slankdev/util.h>
#include <slankdev/exception.h>
#include <slankdev/extra/dpdk.h>

#include <ssn_common.h>
#include <ssn_sys.h>
#include <ssn_timer.h>
#include <ssn_native_thread.h>
#include <ssn_green_thread.h>
#include <ssn_port_stat.h>
#include <ssn_log.h>
#include <ssn_ring.h>
#include <ssn_port.h>


void vnf::deploy()
{
  size_t nb_stages = stages.size();
  for (size_t i=0; i<nb_stages; i++) {
    stages[i]->inc();
  }
}

void vnf::debug_dump(FILE* fp)
{
  fprintf(fp, "vnf1 \r\n");

  size_t nb_stages = stages.size();
  double ruds[nb_stages];
  for (size_t i=0; i<nb_stages; i++) {
    auto name     = stages[i]->name;
    auto pps      = stages[i]->throughput_pps();
    auto mux      = stages[i]->mux();
    auto rud      = stages[i]->reduction_rate();
    ruds[i] = rud;
  }

  size_t min_idx = 0;
  for (size_t i=0; i<nb_stages; i++) {
    if (ruds[i] < ruds[min_idx]) min_idx = i;
  }

  for (size_t i=0; i<nb_stages; i++) {
    auto name     = stages[i]->name;
    auto pps      = stages[i]->throughput_pps();
    auto mux      = stages[i]->mux();
    auto rud      = stages[i]->reduction_rate();
    ruds[i] = rud;
    fprintf(fp ," pl[%zd]: name=\"%s\" mux=%-2zd TP=%-10zd red=%lf",
        i, name.c_str(), mux, pps, rud);
    if (i == min_idx) {
      fprintf(fp, " <-- BOTTLE NECK");
    }
    fprintf(fp, "\r\n");
  }
}

void vnf::tuneup()
{
  printf("scaleup command!! \n");
  printf(" find bottle neck... \n");

  size_t nb_stages = stages.size();
  double ruds[nb_stages];
  for (size_t i=0; i<nb_stages; i++) {
    auto name     = stages[i]->name;
    auto pps      = stages[i]->throughput_pps();
    auto mux      = stages[i]->mux();
    auto rud      = stages[i]->reduction_rate();
    ruds[i] = rud;
  }

  size_t min_idx = 0;
  for (size_t i=0; i<nb_stages; i++) {
    if (ruds[i] < ruds[min_idx]) min_idx = i;
  }

  for (size_t i=0; i<nb_stages; i++) {
    auto name     = stages[i]->name;
    auto pps      = stages[i]->throughput_pps();
    auto mux      = stages[i]->mux();
    auto rud      = stages[i]->reduction_rate();
    ruds[i] = rud;
    printf(" pl[%zd]: name=\"%s\" mux=%-2zd TP=%-10zd red=%lf",
        i, name.c_str(), mux, pps, rud);
    if (i == min_idx) {
      printf(" find! stage[%zd] is bottle neck\n", i);
      stages[i]->inc();
      return ;
    }
    printf("\r\n");
  }
}




