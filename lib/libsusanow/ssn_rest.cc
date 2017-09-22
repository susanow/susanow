
/*
 * MIT License
 *
 * Copyright (c) 2017 Hiroki SHIROKURA
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
/**
 * @file   ssn_rest.cc
 * @brief  rest api
 * @author Hiroki SHIROKURA
 * @date   2017.9.20
 */

#include <ssn_log.h> // for ssn_log() only
#include <ssn_cpu.h> // for ssn_sleep() only
#include <ssn_rest.h>
#include <slankdev/util.h>
#include <slankdev/socketfd.h>
#include <poll.h>

bool ssn_rest_poll_thread_running;

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

Rest_server::Rest_server(uint32_t addr, uint16_t port)
{
  slankdev::socketfd sock;
  sock.noclose_in_destruct = true;

  int yes = 1;
  sock.socket(AF_INET, SOCK_STREAM, 0);
  sock.setsockopt(SOL_SOCKET, SO_REUSEADDR, (const char*)&yes, sizeof(yes));
  sock.bind(addr, port);
  sock.listen(1);

  struct pollfd pfd;
  pfd.fd = sock.get_fd();
  pfd.events = POLLIN | POLLERR;
  fds.push_back(pfd);
}

void Rest_server::add_route(const char* path, ssn_restcb_t callback, void* arg)
{ cbs[std::string(path)] = {callback,arg}; }

void Rest_server::dispatch()
{
  constexpr int notimeout = 0;
  poll(fds.data(), fds.size(), notimeout);

  constexpr int server_sock_idx = 0;
  for (size_t i=0; i<fds.size(); i++) {
    if (fds[i].revents & POLLIN) {
      if (i==server_sock_idx) {

        /*
         * Server Socket
         */
        struct sockaddr_in client;
        socklen_t client_len = sizeof(client);
        int fd = accept(fds[i].fd, (sockaddr*)&client, &client_len);
        if (fd < 0) throw slankdev::exception("accept");
        struct pollfd pfd;
        pfd.fd = fd;
        pfd.events = POLLIN | POLLERR;
        fds.push_back(pfd);
        break;

      } else {

        /*
         * Client Socket
         */
        int fd = fds[i].fd;

        uint8_t buf[1000];
        ssize_t recvlen = read(fd, buf, sizeof(buf));
        if (recvlen < 0) throw slankdev::exception("read");

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
          cbs[uri].first(fd, buf, recvlen, cbs[uri].second);
        }

        fds.erase(fds.begin() + i);
        close(fd);
        break;
      }
    }
  }
}

ssn_rest::ssn_rest(uint32_t addr, uint16_t port) : serv(addr, port) {}

void ssn_rest::add_route(const char* path, ssn_restcb_t cb, void* arg)
{ serv.add_route(path , cb, arg); }

void ssn_rest::dispatch() { serv.dispatch(); }

void ssn_rest_poll_thread(void* arg)
{
  ssn_rest* rest = reinterpret_cast<ssn_rest*>(arg);

  ssn_rest_poll_thread_running = true;
  while (ssn_rest_poll_thread_running) {
    rest->dispatch();
    ssn_yield();
  }
}

void ssn_rest_poll_thread_stop() { ssn_rest_poll_thread_running = false; }


