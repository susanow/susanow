
#pragma once
#include <stdint.h>
#include <stddef.h>
#include <ssn_types.h>

void ssn_native_thread_init();
void ssn_native_thread_fin();
void ssn_native_thread_launch(ssn_function_t f, void* arg, size_t lcore_id);
void ssn_native_thread_waiter(void*);
