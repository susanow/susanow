
#include <stdio.h>
#include <unistd.h>
#include <ssn_sys.h>
#include <ssn_vty.h>

/*-------------------------------------------*/

size_t one=1;
size_t two=2;
size_t three=3;
size_t num0=0;
size_t num1=1;
size_t num2=2;
size_t num3=3;
size_t num4=4;
size_t num5=5;

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

struct slank : public command {
  slank() { nodes.push_back(new node_fixedstring("slank", "")); }
  void func(shell* sh)
  {
    sh->Printf("slankdev\r\n");
  }
};

void ssn_vty_thread(void*)
{
  char str[] = "\r\n"
      "Hello, this is Susanow (version 0.00.00.0).\r\n"
      "Copyright 2017-2020 Hiroki SHIROKURA.\r\n"
      "\r\n"
      " .d8888b.                                                             \r\n"
      "d88P  Y88b                                                            \r\n"
      "Y88b.                                                                 \r\n"
      " \"Y888b.   888  888 .d8888b   8888b.  88888b.   .d88b.  888  888  888 \r\n"
      "    \"Y88b. 888  888 88K          \"88b 888 \"88b d88\"\"88b 888  888  888 \r\n"
      "      \"888 888  888 \"Y8888b. .d888888 888  888 888  888 888  888  888 \r\n"
      "Y88b  d88P Y88b 888      X88 888  888 888  888 Y88..88P Y88b 888 d88P \r\n"
      " \"Y8888P\"   \"Y88888  88888P\' \"Y888888 888  888  \"Y88P\"   \"Y8888888P\"  \r\n"
      "\r\n";

  vty vty0(9999, str, "Susanow> ");
  vty0.install_command(new slank);
  vty0.dispatch();
}

/*-----------------------------------------------------------*/

void timer_test_func(void *arg)
{
  size_t* n = (size_t*)(arg);
  size_t nn = rand()%10;
  printf("timer test %zd \n", nn);
}
void timer_test_func1(void *arg)
{
  size_t* n = (size_t*)(arg);
  size_t nn = rand()%10;
  printf("timer test funcsdfadfa %zd \n", nn);
}

bool running;
void timer_thread(void*)
{
#define TIMER_RESOLUTION_CYCLES 20000000ULL /* around 10ms at 2 Ghz */
  uint64_t cur_tsc;
  uint64_t diff_tsc;
  uint64_t prev_tsc;

  running = true;
  while (running) {
    cur_tsc = rte_rdtsc();
    diff_tsc = cur_tsc - prev_tsc;

    if (diff_tsc > TIMER_RESOLUTION_CYCLES) {
      rte_timer_manage();
      prev_tsc = cur_tsc;
    }
  }
}

void SSN_TIMER_INIT()
{
  rte_timer_subsystem_init();
  ssn_launch(timer_thread, nullptr, 2);
}
struct timer_info {
  rte_timer* tim;
  ssn_function_t f;
  void* arg;
};
std::vector<timer_info*> tim_infos;
void _SSN_TIMER_LAUNCHER(struct rte_timer* tim, void* arg)
{
  timer_info* info = reinterpret_cast<timer_info*>(arg);
  info->f(info->arg);
}
inline void SSN_TIMER_ADD(rte_timer* tim, ssn_function_t f, void* arg)
{
  timer_info* i = new timer_info;
  i->tim = tim;
  i->f   = f;
  i->arg = arg;
  tim_infos.push_back(i);
	uint64_t hz = rte_get_timer_hz();
  rte_timer_reset(tim, hz, PERIODICAL, 2, _SSN_TIMER_LAUNCHER, i);
}
void SSN_TIMER_DEL(rte_timer* tim)
{
  rte_timer_stop(tim);
  for (size_t i=0; i<tim_infos.size(); i++) {
    if (tim == tim_infos[i]->tim) {
      timer_info* info = tim_infos[i];
      tim_infos.erase(tim_infos.begin() + i);
      delete info;
      return ;
    }
  }
  throw slankdev::exception("not found timer obj to delete");
}

void Slankdev1(rte_timer* tim, void*) { printf("slankdev1\n"); }
void Slankdev(rte_timer* tim, void*) { printf("slankdev\n"); }
void WSlankdev1(void*) { printf("slankdev1\n"); }
void WSlankdev (void*) { printf("slankdev\n"); }

int main(int argc, char** argv)
{
  ssn_init(argc, argv);
  // ssn_ltsched_register(1);
  // ssn_launch(ssn_vty_thread    , nullptr, 1);
  // ssn_launch(ssn_waiter_thread , nullptr, 1);
  SSN_TIMER_INIT();

  rte_timer tim;
  rte_timer tim1;
  rte_timer_init(&tim);
  rte_timer_init(&tim1);

  SSN_TIMER_ADD(&tim , WSlankdev, nullptr);
  SSN_TIMER_ADD(&tim1, WSlankdev1, nullptr);
  sleep(3);
  sleep(1); SSN_TIMER_DEL(&tim);
  sleep(1); SSN_TIMER_DEL(&tim1);
  running = false;
  rte_eal_mp_wait_lcore();
}


