
#include <queue>
#include <dlfcn.h>
#include <unistd.h>
#include <lthread.h>
#include <slankdev/extra/dpdk.h>
#include <slankdev/extra/dpdk_struct.h>
#include <slankdev/hexdump.h>

#include <ssn_sys.h>


ssn_sys sys;

int _fthread_launcher(void* arg)
{
  ssn_lcore* lcore = reinterpret_cast<ssn_lcore*>(arg);
  lcore->state = SSN_LS_RUNNING_NATIVE;
  lcore->f(lcore->arg);
  lcore->state = SSN_LS_FINISHED;
  return 0;
}

void lthread_ctrl(void* arg)
{
  ssn_lthread_sched* sched = reinterpret_cast<ssn_lthread_sched*>(arg);
  sched->running = true;
  while (sched->running) {

    for (size_t i=0; i<sched->threads.size(); i++) {
      int ret = lthread_join(sched->threads[i].lt, nullptr, 1);
      if (ret == 0 || ret == -1) {
        sched->threads[i].dead = true;
      }
    }
    for (size_t i=0; i<sched->threads.size(); i++) {
      if (sched->threads[i].dead) {
        sched->threads.erase(sched->threads.begin() + i);
        i--;
      }
    }

    if (!sched->launch_queue.empty()) {
      launch_info ci = sched->launch_queue.front();
      sched->launch_queue.pop();
      lthread* th;
      lthread_create(&th, ci.f, ci.arg);
      sched->threads.push_back({th, ci.f, ci.arg});
    }
    lthread_sleep(1);
  }
}

int _lthread_launcher(void* arg)
{
  uint32_t lid = rte_lcore_id();
  ssn_lthread_sched* sched = reinterpret_cast<ssn_lthread_sched*>(arg);
  lthread* lt;
  lthread_create(&lt, lthread_ctrl, arg);
  sched->threads.push_back({lt, lthread_ctrl, arg, false});
  lthread_run();
  delete sys.cpu.lcores[lid].lt_sched;
  sys.cpu.lcores[lid].lt_sched = nullptr;
}

void ssn_lthread_sched::start_scheduler(uint32_t lcore_id)
{ rte_eal_remote_launch(_lthread_launcher, this, lcore_id); }
void ssn_lthread_sched::stop_scheduler() { running = false; }
void ssn_lthread_sched::launch(ssn_function_t f, void* a) { launch_queue.push({f, a}); }
ssn_lthread_sched::ssn_lthread_sched() : running(false) {}
void ssn_lthread_sched::debug_dump(FILE* fp)
{
  fprintf(fp, "ltsched: %p  \r\n", this);
  fprintf(fp, "%5s: %20s %20s(%30s) %20s\r\n", "idx", "lt", "f", "name", "arg");
  fprintf(fp, "-----------------------------------------------------------");
  fprintf(fp, "-------------------------------------------------\r\n");
  for (size_t i=0; i<threads.size(); i++) {
    Dl_info dli;
    dladdr((void*)threads[i].f, &dli);

    fprintf(fp, "[%3zd]: %20p %20p(%30s) %20p\r\n", i,
        threads[i].lt, threads[i].f, dli.dli_sname, threads[i].arg);
  }
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
  if (lt_sched) {
    lt_sched->debug_dump(fp);
  }
}

void ssn_lcore::lthread_sched_register()
{
  lt_sched =  new ssn_lthread_sched;
  if (!lt_sched) throw slankdev::exception("new ssn_lthread_sched");
  state = SSN_LS_RUNNING_LTHREAD;
  lt_sched->start_scheduler(id);
}

void ssn_lcore::lthread_sched_unregister()
{
  lt_sched->stop_scheduler();
}

void ssn_lcore::launch(ssn_function_t _f, void* _arg)
{
  if (lt_sched) {
    lt_sched->launch(_f, _arg);
  } else {
    f =  _f;
    arg = _arg;
    rte_eal_remote_launch(_fthread_launcher, this, id);
  }
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
  for (size_t i=0; i<lcores.size(); i++) {
    lcores[i].debug_dump(fp);
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

