

#include <string.h>
#include <slankdev/util.h>
#include <slankdev/rest.h>


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
  slankdev::rest_server serv(INADDR_ANY, 80);
  serv.add_route("/"       , callback_root   , nullptr);
  serv.add_route("/stats"  , callback_stats  , nullptr);
  serv.add_route("/author" , callback_authorr, nullptr);
  serv.dispatch();
}


