

#include <susanow.h>

void ssn_sleep(size_t msec)
{
  if (is_lthread(rte_lcore_id())) lthread_sleep(msec);
  else usleep(msec * 1000);
}

void ssn_init(int argc, char** argv)
{ sys.init(argc, argv); }

void ssn_launch(ssn_function_t f, void* arg, size_t lcore_id)
{ sys.cpu.lcores[lcore_id].launch(f, arg); }

void ssn_wait(size_t lcore_id) { sys.cpu.lcores[lcore_id].wait(); }
ssn_sys* ssn_get_sys() { return &sys; }
bool ssn_cpu_debug_dump(FILE* fp) { sys.cpu.debug_dump(fp); }

ssn_lcore_state ssn_get_lcore_state(size_t lcore_id)
{ return sys.cpu.lcores[lcore_id].state; }
bool is_lthread(size_t lcore_id)
{ return ssn_get_lcore_state(lcore_id) == SSN_LS_RUNNING_LTHREAD; }
bool is_tthread(size_t lcore_id)
{ return ssn_get_lcore_state(lcore_id) == SSN_LS_RUNNING_TIMER; }
