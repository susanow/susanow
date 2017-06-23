

#include <stdio.h>
#include <unistd.h>
#include <susanow.h>
#include "ssn_port.h"

size_t num0=0;
size_t num1=1;
size_t num2=2;
size_t num3=3;
size_t num4=4;
size_t num5=5;

int main(int argc, char** argv)
{
  ssn_init(argc, argv);
  ssn_port_init();

  size_t nb_ports = ssn_dev_count();
  printf("%zd ports found \n", nb_ports);

  for (size_t i=0; i<nb_ports; i++) {
    ssn_port_init(i, 1, 1);
    ssn_port_linkup(i);
  }

  ssn_port_fin();
  ssn_fin();
}


