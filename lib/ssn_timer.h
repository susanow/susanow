
#pragma once
#include <stdint.h>
#include <stddef.h>
#include <ssn_sys.h>

class ssn_timer;

void ssn_timer_init();
void ssn_timer_fin();
void ssn_timer_sched_register(size_t lcore_id);
void ssn_timer_sched_unregister(size_t lcore_id);
ssn_timer* ssn_timer_alloc(ssn_function_t f, void* arg, size_t hz);
void ssn_timer_free(ssn_timer* st);
void ssn_timer_add(ssn_timer* tim, size_t lcore_id);
void ssn_timer_del(ssn_timer* st, size_t lcore_id);
void ssn_timer_debug_dump(FILE* fp, size_t lcore_id);
void ssn_timer_debug_dump(FILE* fp);

