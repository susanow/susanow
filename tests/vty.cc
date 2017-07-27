
#include <ssn_vty.h>
#include <ssn_green_thread.h>
#include <ssn_native_thread.h>
#include <ssn_sys.h>
#include <ssn_common.h>
#include <slankdev/util.h>
#include <slankdev/socketfd.h>

/* slank */
vty_cmd_match slank_mt()
{
  vty_cmd_match m;
  m.nodes.push_back(new node_fixedstring("slank", ""));
  return m;
}
void slank_f(vty_cmd_match* m, vty_client* sh, void* arg)
{
  size_t* n = reinterpret_cast<size_t*>(arg);
  sh->Printf("slankdev called %zd times\r\n", (*n)++);
}

int main(int argc, char** argv)
{
  size_t n = 0;
  ssn_init(argc, argv);

  ssn_vty vty(INADDR_ANY, 8888);
  vty.install_command(slank_mt(), slank_f, &n);

  ssn_green_thread_sched_register(1);
  ssn_green_thread_launch(ssn_vty_poll_thread, &vty, 1);
  getchar();
  ssn_vty_poll_thread_stop();
  ssn_green_thread_sched_unregister(1);

  ssn_wait_all_lcore();
  ssn_fin();
}
