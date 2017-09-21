
#pragma once

#include <ssn_native_thread.h>
#include <ssn_green_thread.h>

uint16_t ssn_thread_launch(ssn_function_t f, void* arg, size_t lcore_id);
void ssn_thread_join(uint16_t tid);
void ssn_thread_debug_dump(FILE* fp);
void ssn_thread_wait(uint16_t tid);
