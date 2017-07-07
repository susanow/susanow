
#include <susanow.h>
#include <slankdev/util.h>
#include <slankdev/socketfd.h>



int main(int argc, char** argv)
{
  ssn_init(argc, argv);

  ssn_vty vty(INADDR_ANY, 8888);

  ssn_native_thread_launch(ssn_vty_poll_thread, &vty, 1);
  getchar();
  ssn_vty_poll_thread_stop();

  ssn_wait_all_lcore();
  ssn_fin();
}
