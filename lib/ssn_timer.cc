

#include <assert.h>
#include <susanow.h>
#include <ssn_sys.h>
#include <ssn_timer.h>
#include <ssn_native_thread.h>
#include <slankdev/extra/dpdk.h>


class ssn_timer {
 public:
  rte_timer tim;
  ssn_function_t f;
  void* arg;
  size_t hz;
  ssn_timer(ssn_function_t _f, void* _arg, size_t _hz)
    : f(_f), arg(_arg), hz(_hz) { rte_timer_init(&tim); }
  virtual ~ssn_timer() {}
};


static void timer_thread(void* arg);
class ssn_timer_manager {
  friend void timer_thread(void* arg);
 private:
  size_t lcore_id;
  bool timer_running;
  std::vector<ssn_timer*> tims;
 public:
  ssn_timer_manager(size_t i) : lcore_id(i), timer_running(false) {}
  virtual ~ssn_timer_manager() {}

  void sched_register();
  void sched_unregister();
  void add(ssn_timer* tim);
  void del(ssn_timer* tim);
  void debug_dump(FILE* fp);


};
ssn_timer_manager* stm[RTE_MAX_LCORE];

void _SSN_TIMER_LAUNCHER(struct rte_timer* tim, void* arg)
{
  ssn_timer* st = reinterpret_cast<ssn_timer*>(arg);
  st->f(st->arg);
}

void ssn_timer_manager::sched_register()
{
  ssn_native_thread_launch(timer_thread, this, lcore_id);
  ssn_set_lcore_state(SSN_LS_RUNNING_TIMER, lcore_id);
}
void ssn_timer_manager::sched_unregister()
{
  ssn_set_lcore_state(SSN_LS_RUNNING_NATIVE, lcore_id);
  timer_running = false;
}
void ssn_timer_manager::add(ssn_timer* tim)
{
  tims.push_back(tim);
  if (!is_tthread(lcore_id)) {
    ssn_lcore_state s = ssn_get_lcore_state(lcore_id);
    std::string e;
    e += "ssn_timer_add: ";
    e += slankdev::format("lcore%zd is not timer thread", lcore_id);
    e += slankdev::format(" (current: %s)", ssn_lcore_state2str(s));
    throw slankdev::exception(e.c_str());
  }
  rte_timer_reset(&tim->tim, tim->hz, PERIODICAL, lcore_id, _SSN_TIMER_LAUNCHER, tim);
}
void ssn_timer_manager::del(ssn_timer* tim)
{
  if (!is_tthread(lcore_id)) {
    ssn_lcore_state s = ssn_get_lcore_state(lcore_id);
    std::string e;
    e += "ssn_timer_del: ";
    e += slankdev::format("lcore%zd is not timer thread", lcore_id);
    e += slankdev::format(" (current: %s)", ssn_lcore_state2str(s));
    throw slankdev::exception(e.c_str());
  }
  rte_timer_stop(&tim->tim);

  for (size_t i=0; i<tims.size(); i++) {
    if (tims[i] == tim) {
      tims.erase(tims.begin() + i);
      return ;
    }
  }
  throw slankdev::exception("not found timer object");
}
void ssn_timer_manager::debug_dump(FILE* fp)
{
  fprintf(fp, "ssn_timer\r\n");
  fprintf(fp, "%5s: %-15s %-15s %-15s \r\n", "idx", "rte_timer", "funcptr", "arg");
  fprintf(fp, "--------------------------------------");
  fprintf(fp, "-----------------------------------\r\n");
  for (size_t i=0; i<tims.size(); i++) {
    fprintf(fp, "[%3zd]: %-15p %-15p %-15p \r\n", i,
        &tims[i]->tim,
         tims[i]->f,
         tims[i]->arg);
  }
}

static void timer_thread(void* arg)
{
  ssn_timer_manager* mgr = reinterpret_cast<ssn_timer_manager*>(arg);

  constexpr size_t TIMER_RESOLUTION_CYCLES = 20000000ULL; /* around 10ms at 2 Ghz */
  uint64_t cur_tsc;
  uint64_t diff_tsc;
  uint64_t prev_tsc;

  size_t lcore_id = mgr->lcore_id;
  assert(lcore_id == rte_lcore_id());
  mgr->timer_running = true;
  while (mgr->timer_running) {
    cur_tsc = rte_rdtsc();
    diff_tsc = cur_tsc - prev_tsc;

    if (diff_tsc > TIMER_RESOLUTION_CYCLES) {
      rte_timer_manage();
      prev_tsc = cur_tsc;
    }
  }
}

void ssn_timer_init()
{
  rte_timer_subsystem_init();
  size_t nb = rte_lcore_count();
  for (size_t i=0; i<nb; i++) {
    stm[i] = new ssn_timer_manager(i);
  }
}
void ssn_timer_fin()
{
  size_t nb = rte_lcore_count();
  for (size_t i=0; i<nb; i++) {
    delete stm[i];
  }
}
void ssn_timer_sched_register(size_t lcore_id) { stm[lcore_id]->sched_register(); }
void ssn_timer_sched_unregister(size_t lcore_id) { stm[lcore_id]->sched_unregister(); }
void ssn_timer_del(ssn_timer* tim, size_t lcore_id)  { stm[lcore_id]->del(tim); }
void ssn_timer_add(ssn_timer* tim, size_t lcore_id) { stm[lcore_id]->add(tim); }
ssn_timer* ssn_timer_alloc(ssn_function_t f, void* arg, size_t hz) { return new ssn_timer(f, arg, hz); }
void ssn_timer_free(ssn_timer* st) { delete st ; }
void ssn_timer_debug_dump(FILE* fp, size_t lcore_id) { stm[lcore_id]->debug_dump(fp); }
void ssn_timer_debug_dump(FILE* fp)
{
  size_t nb_lcores = rte_lcore_count();
  for (size_t i=0; i<nb_lcores; i++) {
    ssn_timer_debug_dump(fp, i);
  }
}

