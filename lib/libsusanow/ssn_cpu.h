
#pragma once
#include <vector>
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <ssn_types.h>



void ssn_cpu_init(int argc, char** argv);
bool ssn_cpu_debug_dump(FILE* fp);

ssn_lcore_state ssn_get_lcore_state(size_t lcore_id);
void ssn_set_lcore_state(ssn_lcore_state s, size_t lcore_id);

size_t ssn_lcore_id();
size_t ssn_lcore_count();

bool is_green_thread(size_t lcore_id);
bool is_tthread(size_t lcore_id);

void ssn_sleep(size_t msec);

void ssn_yield();
