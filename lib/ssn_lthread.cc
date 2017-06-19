

#include <susanow.h>
#include <ssn_lthread.h>
#include <dlfcn.h>

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
  fprintf(fp, "---------------------------------------------------");
  fprintf(fp, "-------------------------------------------------\r\n");
  for (size_t i=0; i<threads.size(); i++) {
    Dl_info dli;
    dladdr((void*)threads[i].f, &dli);

    fprintf(fp, "[%3zd]: %20p %20p(%30s) %20p\r\n", i,
        threads[i].lt, threads[i].f, dli.dli_sname, threads[i].arg);
  }
}

