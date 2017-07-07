

#include <string.h>
#include <slankdev/util.h>
#include <slankdev/rest.h>


void callback_root(int fd, const void* buf, size_t len, void*)
{ slankdev::fdprintf(fd, "{ \"kanailab\" : \"Wonderful OBAKA Laboratory\" }\n"); }

void callback_slankdev(int fd, const void* buf, size_t len, void*)
{ slankdev::fdprintf(fd, "{ \"slankdev\" : \"Hiroki SHIROKURA\" }\n"); }

void callback_yukaribonk(int fd, const void* buf, size_t len, void*)
{ slankdev::fdprintf(fd, "{ \"yukaribonk\" : \"Yukari KUBO\" }\n"); }

void callback_stats(int fd, const void* buf, size_t len, void*)
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
  serv.add_route("/", callback_root);
  serv.add_route("/slankdev", callback_slankdev);
  serv.add_route("/yukaribonk", callback_yukaribonk);
  serv.add_route("/stats" , callback_stats);
  serv.dispatch(nullptr);
}

