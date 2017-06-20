
#pragma once
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>

class ssn_sys;

void ssn_sleep(size_t msec);

void ssn_init(int argc, char** argv);
void ssn_launch(ssn_function_t f, void* arg, size_t lcore_id);
void ssn_wait(size_t lcore_id);
ssn_sys* ssn_get_sys();
bool ssn_cpu_debug_dump(FILE* fp);

ssn_lcore_state ssn_get_lcore_state(size_t lcore_id);
bool is_lthread(size_t lcore_id);
bool is_tthread(size_t lcore_id);


