
#include <vnf.h>

#include <string>
#include <vector>
#include <stdio.h>

#include <slankdev/util.h>
#include <slankdev/exception.h>
#include <dpdk/dpdk.h>

#include <ssn_common.h>
#include <ssn_cpu.h>
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

  size_t stage_perf[nb_stages];
  for (size_t i=0; i<nb_stages; i++) {
    stage_perf[i] = stages[i]->throughput_pps();
  }

  for (size_t i=0; i<nb_stages; i++) {
    void* ptr  = stages[i];
    size_t mux = stages[i]->mux();
    double r = 1.0;
    if (i!=0) r = double(stage_perf[i])/double(stage_perf[i-1]);
    fprintf(fp ," pl[%zd]: name=\"%s\" mux=%-2zd TP=%-10zd red=%lf",
        i, stages[i]->name.c_str(),
        stages[i]->mux(),
        stages[i]->throughput_pps(), r);
    if (r < vnf::THRESH) {
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
  size_t stage_perf[nb_stages];
  for (size_t i=0; i<nb_stages; i++) {
    stage_perf[i] = stages[i]->throughput_pps();
  }
  for (size_t i=0; i<nb_stages; i++) {
    double r = 1.0;
    if (i!=0) r = double(stage_perf[i])/double(stage_perf[i-1]);
    if (r < vnf::THRESH) {
      printf(" find! stage[%zd] is bottle neck\n", i);
      stages[i]->inc();
      return ;
    }
  }
  printf(" not found bottle neck. return \n");
}



