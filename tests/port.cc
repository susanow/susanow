

#include <stdio.h>
#include <unistd.h>
#include <ssn_port.h>
#include <ssn_timer.h>
#include <ssn_port_stat.h>
#include <ssn_sys.h>
#include <ssn_common.h>


int main(int argc, char** argv)
{
  ssn_init(argc, argv);

  ssn_port_conf conf;
  size_t nb_ports = ssn_dev_count();

  printf("---------------------------------------\n");
  for (size_t i=0; i<nb_ports; i++) {
    ssn_port_configure(i, &conf);
    ssn_port_dev_up(i);
    ssn_port_link_up(i);
    ssn_port_promisc_on(i);
  }
  printf("---------------------------------------\n");
  for (size_t i=0; i<nb_ports; i++) {
    ssn_port_configure(i, &conf);
    ssn_port_dev_up(i);
    ssn_port_link_up(i);
    ssn_port_promisc_on(i);
  }
  printf("---------------------------------------\n");

  ssn_fin();
}


