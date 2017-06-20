
#include <susanow.h>
#include <ssn_sys.h>
#include <ssn_timer.h>


bool timer_running[RTE_MAX_LCORE] = {false};
std::vector<ssn_timer*> tims;

static void timer_thread(void*)
{
  constexpr size_t TIMER_RESOLUTION_CYCLES = 20000000ULL; /* around 10ms at 2 Ghz */
  uint64_t cur_tsc;
  uint64_t diff_tsc;
  uint64_t prev_tsc;

  size_t lcore_id = rte_lcore_id();
  timer_running[lcore_id] = true;
  while (timer_running[lcore_id]) {
    cur_tsc = rte_rdtsc();
    diff_tsc = cur_tsc - prev_tsc;

    if (diff_tsc > TIMER_RESOLUTION_CYCLES) {
      rte_timer_manage();
      prev_tsc = cur_tsc;
    }
  }
}

void ssn_tmsched_register(size_t lcore_id)
{
  rte_timer_subsystem_init();
  ssn_launch(timer_thread, nullptr, lcore_id);
  sys.cpu.lcores[lcore_id].state = SSN_LS_RUNNING_TIMER;
}

void ssn_tmsched_unregister(size_t lcore_id)
{
  timer_running[lcore_id] = false;
  sys.cpu.lcores[lcore_id].state = SSN_LS_RUNNING_NATIVE;
}

void _SSN_TIMER_LAUNCHER(struct rte_timer* tim, void* arg)
{
  ssn_timer* st = reinterpret_cast<ssn_timer*>(arg);
  st->f(st->arg);
}

ssn_timer* ssn_timer_alloc(ssn_function_t f, void* arg,
                              size_t hz, size_t lcore_id)
{
  if (!is_tthread(lcore_id)) {
    ssn_lcore_state s = ssn_get_lcore_state(lcore_id);
    std::string e;
    e += "ssn_timer_alloc: ";
    e += slankdev::format("lcore%zd is not timer thread", lcore_id);
    e += slankdev::format(" (current: %s)", ssn_lcore_state2str(s));
    throw slankdev::exception(e.c_str());
  }
  ssn_timer* t = new ssn_timer(f, arg, hz, lcore_id);
  rte_timer_init(&t->tim);
  tims.push_back(t);
  return t;
}

void ssn_timer_free(ssn_timer* st)
{
  for (size_t i=0; i<tims.size(); i++) {
    if (tims[i] == st) {
      tims.erase(tims.begin() + i);
      delete st ;
      return ;
    }
  }
  throw slankdev::exception("not found timer object");
}

void ssn_timer_add(ssn_timer* tim)
{
  size_t lcore_id = tim->lcore_id;
  if (!is_tthread(lcore_id)) {
    ssn_lcore_state s = ssn_get_lcore_state(lcore_id);
    std::string e;
    e += "ssn_timer_add: ";
    e += slankdev::format("lcore%zd is not timer thread", lcore_id);
    e += slankdev::format(" (current: %s)", ssn_lcore_state2str(s));
    throw slankdev::exception(e.c_str());
  }
  rte_timer_reset(&tim->tim, tim->hz, PERIODICAL, tim->lcore_id, _SSN_TIMER_LAUNCHER, tim);
}

void ssn_timer_del(ssn_timer* st)
{
  size_t lcore_id = st->lcore_id;
  if (!is_tthread(lcore_id)) {
    ssn_lcore_state s = ssn_get_lcore_state(lcore_id);
    std::string e;
    e += "ssn_timer_del: ";
    e += slankdev::format("lcore%zd is not timer thread", lcore_id);
    e += slankdev::format(" (current: %s)", ssn_lcore_state2str(s));
    throw slankdev::exception(e.c_str());
  }
  rte_timer_stop(&st->tim);
}

void ssn_timer_debug_dump(FILE* fp)
{
  fprintf(fp, "ssn_timer\r\n");
  fprintf(fp, "%5s: %-15s %-15s %-15s %-10s\r\n", "idx", "rte_timer", "funcptr", "arg", "lcore");
  fprintf(fp, "--------------------------------------");
  fprintf(fp, "-----------------------------------\r\n");
  for (size_t i=0; i<tims.size(); i++) {
    fprintf(fp, "[%3zd]: %-15p %-15p %-15p %-10zd \r\n", i, &tims[i]->tim, tims[i]->f, tims[i]->arg, tims[i]->lcore_id);
  }
}




