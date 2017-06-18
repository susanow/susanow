
#include <stdio.h>
#include <unistd.h>
#include <ssn_sys.h>
#include <ssn_vty.h>

/*-------------------------------------------*/

size_t one=1,two=2,three=3;

void func(void* arg)
{
  std::string* id = (std::string*)arg;
  for (size_t i=0; i<13; i++) {
    printf("%u %s\n", rte_lcore_id(), id->c_str());
    ssn_sleep(1000);
  }
}

void ssn_waiter_thread(void*)
{
  size_t nb_lcores = sys.cpu.lcores.size();
  while (true) {
    for (size_t i=0; i<nb_lcores; i++) {
      ssn_wait(i);
      ssn_sleep(1);
    }
  }
}

struct slank : public command {
  slank() { nodes.push_back(new node_fixedstring("slank", "")); }
  void func(shell* sh)
  {
    sh->Printf("slankdev\r\n");
  }
};

void ssn_vty_thread(void*)
{
  char str[] = "\r\n"
      "Hello, this is Susanow (version 0.00.00.0).\r\n"
      "Copyright 2017-2020 Hiroki SHIROKURA.\r\n"
      "\r\n"
      " .d8888b.                                                             \r\n"
      "d88P  Y88b                                                            \r\n"
      "Y88b.                                                                 \r\n"
      " \"Y888b.   888  888 .d8888b   8888b.  88888b.   .d88b.  888  888  888 \r\n"
      "    \"Y88b. 888  888 88K          \"88b 888 \"88b d88\"\"88b 888  888  888 \r\n"
      "      \"888 888  888 \"Y8888b. .d888888 888  888 888  888 888  888  888 \r\n"
      "Y88b  d88P Y88b 888      X88 888  888 888  888 Y88..88P Y88b 888 d88P \r\n"
      " \"Y8888P\"   \"Y88888  88888P\' \"Y888888 888  888  \"Y88P\"   \"Y8888888P\"  \r\n"
      "\r\n";

  vty vty0(9999, str, "Susanow> ");
  vty0.install_command(new slank);
  vty0.dispatch();
}

/*-----------------------------------------------------------*/

int main(int argc, char** argv)
{
  ssn_init(argc, argv);
  ssn_ltsched_register(1);

  std::string str[2];
  str[0] = "test0";
  str[1] = "test1";

  ssn_launch(func              , &str[0], 1);
  ssn_launch(func              , &str[1], 1);
  ssn_launch(ssn_vty_thread    , nullptr, 1);
  ssn_launch(ssn_waiter_thread , nullptr, 1);
  // sleep(15);
  // ssn_ltsched_unregister(1);

  rte_eal_mp_wait_lcore();
}
