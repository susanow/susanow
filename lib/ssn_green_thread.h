
#pragma once
#include <ssn_types.h>

void ssn_green_thread_init();
void ssn_green_thread_fin();
void ssn_green_thread_sched_register(size_t lcore_id);
void ssn_green_thread_sched_unregister(size_t lcore_id);
void ssn_green_thread_launch(ssn_function_t f, void* arg, size_t lcore_id);
void ssn_green_thread_debug_dump(FILE* fp, size_t lcore_id);
void ssn_green_thread_debug_dump(FILE* fp);

