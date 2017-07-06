

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <thread>
#include <slankdev/filefd.h>

#include "ssn_webui.h"

// #define PATH "out.json"
#define PATH "../webui/ssn.json"

bool running = true;
void update(void* arg)
{
  ssn_stats* s = reinterpret_cast<ssn_stats*>(arg);
  while (running) {
    slankdev::filefd file;
    file.fopen(PATH, "w+");
    s->dump(file.getfp());
    usleep(1000);
  }
}
void control(void* arg)
{
  ssn_stats* s = reinterpret_cast<ssn_stats*>(arg);
  while (running) {
    char c = getchar();
    if (c == 'q') break;
    s->nb_threads += 1;
    s->dump(stdout);
  }
  running = false;
}
int main(int argc, char** argv)
{
  ssn_stats* s = new ssn_stats;
  s->nb_threads = 1;
  s->nb_cpus    = 2;
  s->nb_ports   = 3;
  s->author     = std::string("Hiroki SHIROKURA");
  s->vnfs.resize(2);
  s->vnfs[0].id = 123;
  s->vnfs[0].name = std::string("slankdev");
  s->vnfs[0].stages = 3;
  s->vnfs[1].id = 456;
  s->vnfs[1].name = std::string("yukaribonk");
  s->vnfs[1].stages = 5;
  s->dump(stdout);

  std::thread t0(update , s);
  std::thread t1(control, s);
  t0.join();
  t1.join();
}


