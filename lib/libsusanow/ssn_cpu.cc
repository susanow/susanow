
/*
 * MIT License
 *
 * Copyright (c) 2017 Hiroki SHIROKURA
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include <dlfcn.h>
#include <unistd.h>
#include <ssn_cpu.h>
#include <lthread.h>
#include <slankdev/hexdump.h>
#include <slankdev/exception.h>
#include <dpdk/dpdk.h>


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


ssn_cpu cpu;


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
      dpdk::rte_lcore_state_t2str(s));
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
          dpdk::rte_lcore_state_t2str(rs), ssn_lcore_state2str(ss));
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


void ssn_cpu_init(int argc, char** argv)
{
  dpdk::dpdk_boot(argc, argv);
  cpu.init(rte_lcore_count());
}
bool ssn_cpu_debug_dump(FILE* fp) { cpu.debug_dump(fp); }

ssn_lcore_state ssn_get_lcore_state(size_t lcore_id)
{ return cpu.lcores[lcore_id].state; }
void ssn_set_lcore_state(ssn_lcore_state s, size_t lcore_id)
{ cpu.lcores[lcore_id].state = s; }

bool ssn_lcoreid_is_green_thread(size_t lcore_id) { return ssn_get_lcore_state(lcore_id) == SSN_LS_RUNNING_GREEN; }
bool ssn_lcoreid_is_tthread(size_t lcore_id) { return ssn_get_lcore_state(lcore_id) == SSN_LS_RUNNING_TIMER; }

void ssn_sleep(size_t msec)
{
  if (ssn_lcoreid_is_green_thread(rte_lcore_id())) lthread_sleep(msec*1000000);
  else usleep(msec * 1000);
}

void ssn_yield()
{ if (ssn_lcoreid_is_green_thread(rte_lcore_id())) lthread_yield(); }


