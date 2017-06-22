
#pragma once
#include <ssn_types.h>

void ssn_lthread_init();
void ssn_lthread_fin();
void ssn_lthread_sched_register(size_t lcore_id);
void ssn_lthread_sched_unregister(size_t lcore_id);
void ssn_lthread_launch(ssn_function_t f, void* arg, size_t lcore_id);
void ssn_lthread_debug_dump(FILE* fp, size_t lcore_id);
void ssn_lthread_debug_dump(FILE* fp);


