

#include <ssn_log.h>
#include <ssn_thread.h>
#include <ssn_common.h>
#include <ssn_cpu.h>

#include <dpdk/dpdk.h>
#include <slankdev/exception.h>


class ssn_thread {
 public:
  uint16_t lcore_id;
  uint16_t tid;
  bool is_native() { return tid==0; }
};
std::vector<ssn_thread> _ssn_threads;


uint16_t ssn_thread_launch(ssn_function_t f, void* arg, size_t lcore_id)
{
  if (lcore_id == 0)
    throw slankdev::exception("ssn_thread_launch: lcore_id is master's id");

  ssn_thread thread;
  void (*thread_launcher)(ssn_function_t, void*, size_t) = nullptr;
  if (is_green_thread(lcore_id)) {

    /*
     * Green Thread
     */
    static size_t gt_cnt_current = 1;
    thread.lcore_id = lcore_id;
    thread.tid = gt_cnt_current++;
    ssn_log(SSN_LOG_DEBUG,
        "ssn_thread_launch: lcore%u is green, %p(%p)\n",
        lcore_id, f, arg);
    thread_launcher = ssn_green_thread_launch;

  } else {

    /*
     * Native Thread
     */
    thread.lcore_id = lcore_id;
    thread.tid = 0;
    ssn_log(SSN_LOG_DEBUG,
        "ssn_thread_launch: lcore%zd is native, %p(%p)\n",
        lcore_id, f, arg);
    thread_launcher = ssn_native_thread_launch;

  }
  _ssn_threads.push_back(thread);
  thread_launcher(f, arg, lcore_id);
  size_t tid = 0;
  return tid;
}

void ssn_thread_wait(uint16_t tid)
{
#if 1
  throw slankdev::exception("Not IMPLE");
  printf("%s(%u)\n", __func__, tid);
  return ;
#else
  if (is_green_thread(lcore_id)) {
    ssn_log(SSN_LOG_DEBUG, "ssn_thread_wait: lcore%zd is green ...\n", lcore_id);
    ssn_log(SSN_LOG_DEBUG, "ssn_thread_wait: lcore%zd is green ...done\n", lcore_id);
  } else {
    ssn_log(SSN_LOG_DEBUG, "ssn_thread_wait: lcore%zd is native ...\n", lcore_id);
    ssn_lcore_join(lcore_id);
    ssn_log(SSN_LOG_DEBUG, "ssn_thread_wait: lcore%zd is native ...done\n", lcore_id);
  }
#endif
}

