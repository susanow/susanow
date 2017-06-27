

#include <stdio.h>
#include <unistd.h>
#include <susanow.h>

size_t num0=0;
size_t num1=1;
size_t num2=2;
size_t num3=3;
size_t num4=4;
size_t num5=5;

bool running;
void l2fwd(void*)
{
  size_t nb_ports = ssn_dev_count();
  printf("start l2fwd nb_ports=%zd \n", nb_ports);
  running = true;
  while (running) {
    for (size_t pid=0; pid<nb_ports; pid++) {
      rte_mbuf* mbufs[32];
      size_t nb_recv = ssn_port_rx_burst(pid, 0, mbufs, 32);
      if (nb_recv == 0) continue;

      size_t nb_send = ssn_port_tx_burst(pid^1, 0, mbufs, nb_recv);
      if (nb_recv > nb_send) {
        ssn_mbuf_free_bulk(&mbufs[nb_send], nb_recv-nb_send);
      }
    }
  }
}

void wait_enter(const char* msg)
{
  printf("%s [Enter]: ", msg);
  getchar();
}

int main(int argc, char** argv)
{
  ssn_init(argc, argv);
  ssn_log_set_level(SSN_LOG_INFO);
  printf("\n\n");

  wait_enter("first");

  ssn_port_conf conf;
  size_t nb_ports = ssn_dev_count();
  for (size_t i=0; i<nb_ports; i++) {
    ssn_port_configure(i, &conf);
    ssn_port_dev_up(i);
    ssn_port_link_up(i);
    ssn_port_promisc_on(i);
  }

  wait_enter("second");
  ssn_launch(l2fwd, nullptr, 2);
  wait_enter("final");
  running = false;
  ssn_sleep(1000);

  printf("after\n");
  ssn_fin();
}


