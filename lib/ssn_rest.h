

#pragma once
#include <susanow.h>
#include <map>
#include <slankdev/util.h>
#include <slankdev/http.h>


static inline std::string getline(const void* buf, size_t len)
{
  std::string s = "";
  const char* p = reinterpret_cast<const char*>(buf);
  for (size_t i=0; i<len; i++) {
    if (p[i] == '\n' || p[i] == EOF || p[i] == '\r') break;
    s += p[i];
  }
  return s;
}

class Rest_server : public slankdev::http {
  struct ele {
    slankdev::http_callback_t f;
    void* arg;
  };
  std::map<std::string,ele> cbs;
 public:

  Rest_server(uint32_t addr, uint16_t port) : slankdev::http(addr, port) { sock.listen(1); }
  void add_route(const char* path, slankdev::http_callback_t callback, void* arg)
  { cbs[std::string(path)] = {callback,arg}; }
  void dispatch()
  {
    struct sockaddr_in client;
    socklen_t client_len = sizeof(client);
    int fd = sock.accept((sockaddr*)&client, &client_len);
    slankdev::socketfd client_sock;
    client_sock.set_fd(fd);

    uint8_t buf[1000];
    size_t recvlen = client_sock.read(buf, sizeof(buf));

    /* Analyze */
    std::string line = getline(buf, recvlen);
    ssn_log(SSN_LOG_DEBUG, "ssn_rest \"%s\" \n", line.c_str());
    char method[256];
    char uri[256];
    sscanf(line.c_str(), "%s %s", method, uri);

    if (cbs.count(uri) == 0) {
      slankdev::fdprintf(fd,
          "HTTP/1.1 400 Bad Request\r\n"
          "Access-Control-Allow-Origin: *\r\n"
          "Content-Type: application/json; charaset=UTF-8\r\n"
          "\r\n"
          "{"
          "   \"error\": {"
          "      \"message\": \"Unsupported get request.\""
          "   }"
          "}"
          );
    } else {
      slankdev::fdprintf(fd,
          "HTTP/1.1 200 OK\r\n"
          "Access-Control-Allow-Origin: *\r\n"
          "Content-Type: application/json; charaset=UTF-8\r\n"
          "\r\n"
          );
      cbs[uri].f(fd, buf, recvlen, cbs[uri].arg);
    }
  }
};

class ssn_rest {
  Rest_server serv;
 public:
  ssn_rest(uint32_t addr, uint16_t port) : serv(addr, port) {}
  void add_route(const char* path, slankdev::http_callback_t cb, void* arg)
  { serv.add_route(path , cb, arg); }
  void dispatch() { serv.dispatch(); }
};

bool ssn_rest_poll_thread_running;
void ssn_rest_poll_thread(void* arg)
{
  ssn_rest* rest = reinterpret_cast<ssn_rest*>(arg);

  ssn_rest_poll_thread_running = true;
  while (ssn_rest_poll_thread_running) {
    rest->dispatch();
    ssn_sleep(1);
  }
}
void ssn_rest_poll_thread_stop() { ssn_rest_poll_thread_running = false; }




