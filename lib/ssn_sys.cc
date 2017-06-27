
#include <dlfcn.h>
#include <unistd.h>
#include <lthread.h>
#include <slankdev/extra/dpdk.h>
#include <slankdev/extra/dpdk_struct.h>
#include <slankdev/hexdump.h>
#include <ssn_sys.h>
#include <susanow.h>
#include <unistd.h>
#include <slankdev/exception.h>
#include <slankdev/extra/dpdk.h>
#include <slankdev/extra/dpdk_struct.h>
#include <slankdev/extra/dpdk.h>
#include <lthread.h>



ssn_sys sys;

int _fthread_launcher(void* arg)
{
  ssn_lcore* lcore = reinterpret_cast<ssn_lcore*>(arg);
  lcore->state = SSN_LS_RUNNING_NATIVE;
  lcore->f(lcore->arg);
  lcore->state = SSN_LS_FINISHED;
  return 0;
}

void ssn_lcore::init(size_t i, ssn_lcore_state s)
{
  id = i;
  state = s;
}

void ssn_lcore::debug_dump(FILE* fp) const
{
  rte_lcore_state_t s = rte_eal_get_lcore_state(id);
  fprintf(fp, "lcore%zd: ssn_state=%s rte_state=%s\r\n",
      id, ssn_lcore_state2str(state),
      slankdev::rte_lcore_state_t2str(s));
}

void ssn_lcore::launch(ssn_function_t _f, void* _arg)
{
  f =  _f;
  arg = _arg;
  rte_eal_remote_launch(_fthread_launcher, this, id);
}

void ssn_lcore::wait()
{
  rte_lcore_state_t s = rte_eal_get_lcore_state(id);
  if (s == FINISHED) {
    rte_eal_wait_lcore(id);
    state = SSN_LS_WAIT;
  }
  if (s == WAIT) {
    state = SSN_LS_WAIT;
  }
}

void ssn_cpu::debug_dump(FILE* fp) const
{
  fprintf(fp, "nb_lcores: %zd \r\n", lcores.size());
  fprintf(fp, "%5s: %-10s %-10s \r\n", "idx", "rte", "ssn");
  fprintf(fp, "--------------------------------------");
  fprintf(fp, "-----------------------------------\r\n");
  for (size_t i=0; i<lcores.size(); i++) {
    rte_lcore_state_t rs = rte_eal_get_lcore_state(i);
    ssn_lcore_state ss = ssn_get_lcore_state(i);
    fprintf(fp, "[%3zd]: %-10s %-10s  \r\n", i,
          slankdev::rte_lcore_state_t2str(rs), ssn_lcore_state2str(ss));
  }

}

void ssn_cpu::init(size_t nb)
{
  for (size_t i=0; i<nb; i++) {
    ssn_lcore lcore;
    lcore.init(i, SSN_LS_WAIT);
    lcores.push_back(lcore);
  }
}

void ssn_sys::init(int argc, char** argv)
{
  slankdev::dpdk_boot(argc, argv);
  cpu.init(rte_lcore_count());
}

void ssn_waiter_thread(void*)
{
  size_t nb_lcores = sys.cpu.lcores.size();
  while (true) {
    for (size_t i=0; i<nb_lcores; i++) {
      ssn_wait(i);
      ssn_sleep(1);
    }
  }
}

ssn_sys* ssn_get_sys() { return &sys; }
void ssn_sys_init(int argc, char** argv) { sys.init(argc, argv); }
void ssn_native_thread_launch(ssn_function_t f, void* arg, size_t lcore_id)
{ sys.cpu.lcores[lcore_id].launch(f, arg); }
void ssn_wait(size_t lcore_id) { sys.cpu.lcores[lcore_id].wait(); }
bool ssn_cpu_debug_dump(FILE* fp) { sys.cpu.debug_dump(fp); }
ssn_lcore_state ssn_get_lcore_state(size_t lcore_id)
{ return sys.cpu.lcores[lcore_id].state; }

