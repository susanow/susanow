

#pragma once
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>

#include <ssn_types.h>
#include <ssn_sys.h>
#include <ssn_timer.h>
#include <ssn_vty.h>
#include <ssn_lthread.h>

void ssn_init(int argc, char** argv);
void ssn_fin();

void ssn_sleep(size_t msec);

bool is_lthread(size_t lcore_id);
bool is_tthread(size_t lcore_id);

