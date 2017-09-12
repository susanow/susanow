
#pragma once

#include <ssn_native_thread.h>
#include <ssn_green_thread.h>

void ssn_thread_wait(size_t lcore_id);
void ssn_thread_launch(ssn_function_t f, void* arg, size_t lcore_id);

