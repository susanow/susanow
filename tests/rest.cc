

#include <string.h>
#include <susanow.h>
#include <slankdev/util.h>
#include <ssn_rest.h>


void callback_root(int fd, const void* buf, size_t len, void* a)
{
  slankdev::fdprintf(fd,
      "{"
         "\"avalable_route\" : ["
            "{ \"route\"   : \"author: get statistics data\" },"
            "{ \"route\"   : \"stats : get author infos\"    } "
         "]"
      "}"
      );
}

void callback_authorr(int fd, const void* buf, size_t len, void* a)
{
  slankdev::fdprintf(fd,
      "{"
        "\"name\"    : \"Hiroki SHIROKURA\"     ,"
        "\"emal\"    : \"slank.dev@gmail.com\"  ,"
        "\"twitter\" : \"@slankdev\"            ,"
        "\"github\"  : \"slankdev\"              "
      "}"
      );
}

void callback_stats(int fd, const void* buf, size_t len, void* a)
{
  slankdev::fdprintf(fd,
      "{"
        "\"nb_vnfs\" : 3,"
        "\"nb_nics\" : 4,"
        "\"nb_thds\" : 5 "
      "}"
      );
}

int main(int argc, char** argv)
{
  ssn_init(argc, argv);

  ssn_rest rest(INADDR_ANY, 8888);
  rest.add_route("/"       , callback_root   , nullptr);
  rest.add_route("/stats"  , callback_stats  , nullptr);
  rest.add_route("/author" , callback_authorr, nullptr);

  ssn_native_thread_launch(ssn_rest_poll_thread, &rest, 1);
  getchar();
  ssn_rest_poll_thread_stop();

  ssn_wait_all_lcore();
  ssn_fin();
}


