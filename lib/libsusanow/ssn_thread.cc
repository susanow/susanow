

#include <ssn_log.h>
#include <ssn_thread.h>
#include <ssn_common.h>
#include <ssn_cpu.h>

#include <dpdk/dpdk.h>
#include <slankdev/exception.h>

void ssn_thread_launch(ssn_function_t f, void* arg, size_t lcore_id)
{
  if (lcore_id == 0)
    throw slankdev::exception("ssn_thread_launch: lcore_id is master's id");

  void (*thread_launcher)(ssn_function_t, void*, size_t) = nullptr;
  if (is_green_thread(lcore_id)) {
    ssn_log(SSN_LOG_DEBUG, "ssn_thread_launch: lcore%zd is green, %p(%p)\n",
        lcore_id, f, arg);
    thread_launcher = ssn_green_thread_launch;
  } else {
    ssn_log(SSN_LOG_DEBUG, "ssn_thread_launch: lcore%zd is native, %p(%p)\n",
        lcore_id, f, arg);
    thread_launcher = ssn_native_thread_launch;
  }
  thread_launcher(f, arg, lcore_id);
}

void ssn_thread_wait(size_t lcore_id)
{
  if (is_green_thread(lcore_id)) {
    ssn_log(SSN_LOG_DEBUG, "ssn_thread_wait: lcore%zd is green ...\n", lcore_id);
    ssn_log(SSN_LOG_DEBUG, "ssn_thread_wait: lcore%zd is green ...done\n", lcore_id);
  } else {
    ssn_log(SSN_LOG_DEBUG, "ssn_thread_wait: lcore%zd is native ...\n", lcore_id);
    ssn_lcore_join(lcore_id);
    ssn_log(SSN_LOG_DEBUG, "ssn_thread_wait: lcore%zd is native ...done\n", lcore_id);
  }
}
