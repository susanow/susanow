
#include <stdio.h>
#include <ssn_log.h>
#include "susanow.h"

#define IMPL \
  while (run) { \
    printf("%s\n",__func__); \
    ssn_sleep(1000); \
    ssn_yield(); \
  }


bool run = true;
void stage1(void*) { IMPL }
void stage2(void*) { IMPL }
void stage3(void*) { IMPL }
void stage4(void*) { IMPL }
void stage5(void*) { IMPL }
void stage6(void*) { IMPL }
void stage7(void*) { IMPL }

int main(int argc, char** argv)
{
  ssn_log_set_level(SSN_LOG_DEBUG);
  ssn s(argc, argv);

  vnf nf1("slankdev");
  nf1.pl.push_back(pipeline_stage(stage1, nullptr));
  nf1.pl.push_back(pipeline_stage(stage2, nullptr));
  s.vnf_deploy(&nf1);

  vnf nf2("yukaribonk");
  nf2.pl.push_back(pipeline_stage(stage3, nullptr));
  nf2.pl.push_back(pipeline_stage(stage4, nullptr));
  nf2.pl.push_back(pipeline_stage(stage5, nullptr));
  s.vnf_deploy(&nf2);

  getchar();
  run = false;
}

