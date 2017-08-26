

#pragma once
#include <stdint.h>
#include <stddef.h>

void ssn_init(int argc, char** argv);
void ssn_fin();
void ssn_wait_all_lcore();

