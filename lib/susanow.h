

#pragma once
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>

#include <ssn_types.h>
#include <ssn_sys.h>
#include <ssn_timer.h>
#include <ssn_vty.h>
#include <ssn_lthread.h>
#include <ssn_log.h>
#include <ssn_port.h>

void ssn_init(int argc, char** argv);
void ssn_fin();

void ssn_yield();
void ssn_sleep(size_t msec);
size_t ssn_lcore_id();

bool is_lthread(size_t lcore_id);
bool is_tthread(size_t lcore_id);

