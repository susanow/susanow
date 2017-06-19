

#pragma once

#include <ssn_types.h>
#include <ssn_sys.h>
#include <ssn_timer.h>
#include <ssn_lthread.h>
#include <ssn_vty.h>

void ssn_sleep(size_t msec);

inline void ssn_init(int argc, char** argv)
{ sys.init(argc, argv); }

inline void ssn_launch(ssn_function_t f, void* arg, size_t lcore_id)
{ sys.cpu.lcores[lcore_id].launch(f, arg); }

inline void ssn_wait(size_t lcore_id) { sys.cpu.lcores[lcore_id].wait(); }
inline ssn_sys* ssn_get_sys() { return &sys; }
inline bool ssn_cpu_debug_dump(FILE* fp) { sys.cpu.debug_dump(fp); }

inline ssn_lcore_state ssn_get_lcore_state(size_t lcore_id)
{ return sys.cpu.lcores[lcore_id].state; }
inline bool is_lthread(size_t lcore_id)
{ return ssn_get_lcore_state(lcore_id) == SSN_LS_RUNNING_LTHREAD; }
inline bool is_tthread(size_t lcore_id)
{ return ssn_get_lcore_state(lcore_id) == SSN_LS_RUNNING_TIMER; }



