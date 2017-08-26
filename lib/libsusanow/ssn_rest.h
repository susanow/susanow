


#pragma once
#include <map>
#include <vector>
#include <poll.h>

typedef void (*ssn_restcb_t)(int,const void*,size_t,void*);

class Rest_server {
  using pr = std::pair<ssn_restcb_t,void*>;
  std::map<std::string,pr> cbs;
  std::vector<struct pollfd> fds;
 public:
  Rest_server(uint32_t addr, uint16_t port);
  void add_route(const char* path, ssn_restcb_t callback, void* arg);
  void dispatch();
};

class ssn_rest {
  Rest_server serv;
 public:
  ssn_rest(uint32_t addr, uint16_t port);
  void add_route(const char* path, ssn_restcb_t cb, void* arg);
  void dispatch();
};

void ssn_rest_poll_thread(void* arg);
void ssn_rest_poll_thread_stop();




