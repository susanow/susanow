
#pragma once
#include <ssn_rest.h>

inline void restcb_root(int fd, const void* buf, size_t len, void* a)
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
inline void restcb_author(int fd, const void* buf, size_t len, void* a)
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
inline void restcb_stats(int fd, const void* buf, size_t len, void* a)
{
  slankdev::fdprintf(fd,
      "{"
        "\"nb_vnfs\" : 3,"
        "\"nb_nics\" : 4,"
        "\"nb_thds\" : 5 "
      "}"
      );
}
