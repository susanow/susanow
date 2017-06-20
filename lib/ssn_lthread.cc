

#include <ssn_lthread.h>

class ssn_lthread {
 public:
  lthread* lt;
  ssn_function_t f;
  void* arg;
  bool dead;

  ssn_lthread(ssn_function_t _f, void* _arg)
    : lt(nullptr), f(_f), arg(_arg), dead(false) {}
  virtual ~ssn_lthread() {}

  void create()
  { lthread_create(&lt, f, arg); }
};

bool lthread_running[RTE_MAX_LCORE] = {false};
std::vector<ssn_lthread*> lthreads[RTE_MAX_LCORE];
std::queue<ssn_lthread*>  pre_launch_lthreads[RTE_MAX_LCORE];

static void _lthread_control(void* arg)
{
  size_t lcore_id = rte_lcore_id();
  std::queue<ssn_lthread*>&  pllts = pre_launch_lthreads[lcore_id];
  std::vector<ssn_lthread*>& lts   = lthreads[lcore_id];

  while (lthread_running[lcore_id]) {
    /*
     * Check and Launch
     */
    while (!pllts.empty()) {
      ssn_lthread* lt = pllts.front();
      pllts.pop();
      lt->create();
      lts.push_back(lt);
    }

    /*
     * Check and Join
     */
    for (size_t i=0; i<lts.size(); i++) {
      int ret = lthread_join(lts[i]->lt, nullptr, 1);
      if (ret == 0 || ret == -1) {
        lts[i]->dead = true;
      }
    }
    for (size_t i=0; i<lts.size(); i++) {
      if (lts[i]->dead) {
        ssn_lthread* _lt = lts[i];
        delete _lt;
        lts.erase(lts.begin() + i);
        i--;
      }
    }
    lthread_sleep(1);
  } // while
}

static void _lthread_starter(void*) {
  ssn_lthread* sl = new ssn_lthread(_lthread_control, nullptr);
  sl->create();
  lthread_run();
  delete sl;
}

void ssn_lthread_sched_register(size_t lcore_id)
{
  sys.cpu.lcores[lcore_id].state = SSN_LS_RUNNING_LTHREAD;
  ssn_launch(_lthread_starter, nullptr, lcore_id);
  lthread_running[lcore_id] = true;
}

void ssn_lthread_sched_unregister(size_t lcore_id)
{
  lthread_running[lcore_id] = false;

  std::queue<ssn_lthread*>&  que = pre_launch_lthreads[lcore_id];
  while (!que.empty()) {
    ssn_lthread* sl = que.front();
    que.pop();
    delete sl;
  }
  std::vector<ssn_lthread*>& vec = lthreads[lcore_id];
  if (vec.size() != 0)
    throw slankdev::exception("There are threads not freed yet");

  sys.cpu.lcores[lcore_id].state = SSN_LS_FINISHED;
}

void ssn_lthread_launch(ssn_function_t f, void* arg, size_t lcore_id)
{
  ssn_lthread* sl = new ssn_lthread(f, arg);
  pre_launch_lthreads[lcore_id].push(sl);
}

void ssn_lthread_debug_dump(FILE* fp, size_t lcore_id)
{
  if (!is_lthread(lcore_id)) throw slankdev::exception("is not lthread");

  std::vector<ssn_lthread*>& vec = lthreads[lcore_id];
  fprintf(fp, "lthread lcore%zd \r\n", lcore_id);
  fprintf(fp, " %5s: %-15s %-15s(%-15s) %-15s \r\n",
      "idx", "lthread", "funcptr", "name", "arg");
  fprintf(fp, " --------------------------------------");
  fprintf(fp, "-----------------------------------\r\n");
  for (size_t i=0; i<vec.size(); i++) {
    Dl_info dli;
    dladdr((void*)vec[i]->f, &dli);
    fprintf(fp, " [%3zd]: %-15p %-15p(%-15s) %-15p \r\n", i, vec[i]->lt,
        vec[i]->f,  dli.dli_sname, vec[i]->arg);
  }
}

void ssn_lthread_debug_dump(FILE* fp)
{
  size_t nb_lcores = rte_lcore_count();
  for (size_t i=0; i<nb_lcores; i++) {
    ssn_lthread_debug_dump(fp, i);
  }
}

