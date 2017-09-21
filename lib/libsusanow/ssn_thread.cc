

#include <ssn_log.h>
#include <ssn_thread.h>
#include <ssn_common.h>
#include <ssn_cpu.h>

#include <dpdk/dpdk.h>
#include <slankdev/exception.h>

bool ssn_tid_is_green_thread(uint32_t tid) { return ((tid&0xffff0000) != 0); }


uint32_t ssn_thread_launch(ssn_function_t f, void* arg, size_t lcore_id)
{
  if (lcore_id == 0)
    throw slankdev::exception("ssn_thread_launch: lcore_id is master's id");

  uint32_t (*thread_launcher)(ssn_function_t, void*, size_t) = nullptr;
  if (is_green_thread(lcore_id)) {
    /*
     * Green Thread
     */
    thread_launcher = ssn_green_thread_launch;
  } else {
    /*
     * Native Thread
     */
    thread_launcher = ssn_native_thread_launch;
  }
  return thread_launcher(f, arg, lcore_id);
}

void ssn_thread_join(uint32_t tid)
{
  if (ssn_tid_is_green_thread(tid)) {
    /*
     * Green Thread
     */
    ssn_green_thread_join(tid);
  } else {
    /*
     * Native Thread
     */
    ssn_native_thread_join(tid);
  }
}

bool ssn_thread_joinable(uint32_t tid)
{
  if (ssn_tid_is_green_thread(tid)) {
    /*
     * Green Thread
     */
    return ssn_green_thread_joinable(tid);
  } else {
    /*
     * Native Thread
     */
    return ssn_native_thread_joinable(tid);
  }
}

void ssn_thread_debug_dump(FILE* fp)
{
  ssn_native_thread_debug_dump(fp);
  ssn_green_thread_debug_dump(fp);
}


