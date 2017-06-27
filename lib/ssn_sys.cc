
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



class ssn_lcore {
 private:
  size_t id;
 public:
  ssn_lcore_state state;

  ssn_lcore() : id(0), state(SSN_LS_WAIT) {}
  ~ssn_lcore() {}

  void init(size_t i, ssn_lcore_state s);
  void debug_dump(FILE* fp) const;
};

class ssn_cpu {
 public:
  std::vector<ssn_lcore> lcores;
  ssn_cpu() {}
  ~ssn_cpu() {}
  void init(size_t nb);
  void debug_dump(FILE* fp) const;
};

class ssn_sys {
 public:
  ssn_cpu cpu;
  ssn_sys() {}
  ~ssn_sys() {}
  void init(int argc, char** argv);
};

ssn_sys sys;


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

void ssn_sys_init(int argc, char** argv) { sys.init(argc, argv); }
bool ssn_cpu_debug_dump(FILE* fp) { sys.cpu.debug_dump(fp); }

ssn_lcore_state ssn_get_lcore_state(size_t lcore_id)
{ return sys.cpu.lcores[lcore_id].state; }
void ssn_set_lcore_state(ssn_lcore_state s, size_t lcore_id)
{ sys.cpu.lcores[lcore_id].state = s; }

size_t ssn_lcore_id() { return rte_lcore_id(); }
size_t ssn_lcore_count() { return rte_lcore_count(); }

bool is_green_thread(size_t lcore_id) { return ssn_get_lcore_state(lcore_id) == SSN_LS_RUNNING_GREEN; }
bool is_tthread(size_t lcore_id) { return ssn_get_lcore_state(lcore_id) == SSN_LS_RUNNING_TIMER; }

void ssn_sleep(size_t msec)
{
  if (is_green_thread(rte_lcore_id())) lthread_sleep(msec*1000000);
  else usleep(msec * 1000);
}

void ssn_yield() { lthread_yield(); }


