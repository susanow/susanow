

#pragma once
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>

#include <ssn_types.h>
#include <ssn_sys.h>
#include <ssn_timer.h>

#include <ssn_vty.h>
#include <ssn_rest.h>

#include <ssn_green_thread.h>
#include <ssn_native_thread.h>
#include <ssn_log.h>
#include <ssn_port.h>
#include <ssn_port_stat.h>

void ssn_init(int argc, char** argv);
void ssn_fin();
void ssn_wait_all_lcore();

