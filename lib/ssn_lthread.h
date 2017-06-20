
#pragma once

#include <stdint.h>
#include <stddef.h>
#include <lthread.h>
#include <dlfcn.h>

#include <vector>
#include <queue>

#include <ssn_sys.h>
#include <ssn_types.h>
#include <ssn_api.h>
#include <slankdev/extra/dpdk.h>


class ssn_lthread;
extern bool lthread_running[RTE_MAX_LCORE];
extern std::vector<ssn_lthread*> lthreads[RTE_MAX_LCORE];
extern std::queue<ssn_lthread*>  pre_launch_lthreads[RTE_MAX_LCORE];


void ssn_lthread_sched_register(size_t lcore_id);
void ssn_lthread_sched_unregister(size_t lcore_id);
void ssn_lthread_launch(ssn_function_t f, void* arg, size_t lcore_id);
void ssn_lthread_debug_dump(FILE* fp, size_t lcore_id);
void ssn_lthread_debug_dump(FILE* fp);


