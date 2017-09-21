
#pragma once

#include <ssn_native_thread.h>
#include <ssn_green_thread.h>

uint32_t ssn_thread_launch(ssn_function_t f, void* arg, size_t lcore_id);
void ssn_thread_join(uint32_t tid);
bool ssn_thread_joinable(uint32_t tid);

void ssn_thread_debug_dump(FILE* fp);
bool ssn_tid_is_green_thread(uint32_t tid);
