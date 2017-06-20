
#pragma once
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>

class ssn_sys;

void ssn_sleep(size_t msec);

bool is_lthread(size_t lcore_id);
bool is_tthread(size_t lcore_id);


