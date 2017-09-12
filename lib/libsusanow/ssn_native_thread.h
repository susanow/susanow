
#pragma once
#include <stdint.h>
#include <stddef.h>
#include <ssn_types.h>

void ssn_native_thread_init();
void ssn_native_thread_fin();
void ssn_native_thread_launch(ssn_function_t f, void* arg, size_t lcore_id);

/*
 * This function return after finish the thread.
 */
void ssn_lcore_join(size_t lcore_id);
bool ssn_lcore_joinable(size_t lcore_id);

void ssn_lcore_join_poll_thread_stop();
void ssn_lcore_join_poll_thread(void*);

