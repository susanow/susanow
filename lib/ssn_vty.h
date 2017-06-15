
/*
 * MIT License
 *
 * Copyright (c) 2017 Susanow
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
 * @file   ssn_vty.h
 * @brief  vty implementation
 * @author Hiroki SHIROKURA
 * @date   2017.6.16
 */



#pragma once

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <unistd.h>
#include <netinet/tcp.h>

#include <string>
#include <vector>
#include <algorithm>

#include <slankdev/socketfd.h>
#include <slankdev/telnet.h>
#include <slankdev/poll.h>
#include <slankdev/exception.h>
#include <slankdev/string.h>
#include <slankdev/util.h>
#include <slankdev/asciicode.h>

#include <ssn_sys.h>




class shell;

class key_func {
 public:
  uint8_t code[256];
  const size_t  len;
  key_func(const void* d, size_t l) : len(l)
  {
    if (sizeof(code) < l) throw slankdev::exception("length is too big");
    memcpy(code, d, l);
  }
  virtual ~key_func() {}
  virtual void function(shell*) = 0;
  virtual bool match(const void* d, size_t l)
  {
    return (len == l) && (memcmp(code, d, l) == 0);
  }
};



class node {
 public:
  virtual bool match(const std::string& str) = 0;
  virtual bool match_prefix(const std::string& str) = 0;
  virtual std::string& get() { throw slankdev::exception("FAAA"); }
  virtual std::string to_string() = 0;
  virtual const std::string& msg() = 0;
};


class node_fixedstring : public node {
  std::string str;
  const std::string msg_;
 public:
  node_fixedstring(const std::string& s, const std::string m)
    : str(s), msg_(m) {}
  bool match(const std::string& s) { return str == s; }
  bool match_prefix(const std::string& s)
  {
    auto ret = str.find(s);
    if (ret != std::string::npos) {
      if (ret == 0) return true;
    }
    return false;
  }
  std::string to_string() { return str; }
  const std::string& msg() { return msg_; }
};


class node_string : public node {
  std::string str;
  const std::string msg_;
 public:
  node_string() : msg_("string") {}
  bool match(const std::string& s)
  {
    if (s.length() > 0) {
      str = s;
      return true;
    }
    return false;
  }
  bool match_prefix(const std::string&)
  { return true; }
  std::string& get() { return str; }
  std::string to_string() { return "<string>"; }
  const std::string& msg() { return msg_; }
};


class shell;
class command {
 public:
  std::vector<node*> nodes;
  virtual ~command() {}
  virtual void func(shell* sh) = 0;
  virtual bool match(const std::string& str);
};




class inputbuffer {
  std::string ibuf;
  size_t cur_idx;
 public:
  inputbuffer() : cur_idx(0) {}
  void input_char(char c) { ibuf.insert(ibuf.begin()+cur_idx, c); cur_idx++; }
  void input_str(const std::string& str) { for (char c : str) input_char(c); }
  void clear()  { ibuf.clear(); cur_idx = 0; }
  size_t length()   const { return ibuf.length(); }
  const char* c_str() const { return ibuf.c_str();  }
  size_t index() const { return cur_idx; }
  bool empty() const { return ibuf.empty(); }
  std::string to_string() const { return ibuf; }

  void cursor_top() { cur_idx = 0; }
  void cursor_end() { cur_idx = ibuf.size(); }
  void cursor_right() { if (cur_idx < ibuf.length()) cur_idx ++ ; }
  void cursor_left() { if (cur_idx > 0) cur_idx -- ; }
  void cursor_backspace()
  {
    if (cur_idx > 0) {
      cur_idx --;
      ibuf.erase(ibuf.begin() + cur_idx);
    }
  }
};



class commandhistory {
  size_t hist_index;
  std::vector<std::string> history;
 public:
  commandhistory() : hist_index(0) {}
  void add(const std::string& str) { history.push_back(str); }
  void clean() { hist_index=0; }
  std::string deep_get()
  {
    if (history.empty()) return "";
    if (hist_index+2 > history.size()) return *(history.end() - hist_index - 1);
    return *(history.end() - ++hist_index);
  }
  std::string shallow_get()
  {
    if (history.empty()) return "";
    if (ssize_t(hist_index)-1 < 0) return *(history.end() - hist_index - 1);
    return *(history.end() - --hist_index - 1);
  }
};



class shell {
  friend class vty;
  std::string prompt;
  int   fd;
  bool  closed;

  void press_keys(const void* d, size_t l);
 public:
  commandhistory history;
  inputbuffer    ibuf;
  const std::vector<command*> * commands;
  const std::vector<key_func*>* keyfuncs;
  void* user_ptr;

  void close() { closed = true; }

  template <class... ARGS> void Printf(const char* fmt, ARGS... args)
  { slankdev::fdprintf(fd, fmt, args...); }

  shell(int d, const char* bootmsg, const char* prmpt,
      const std::vector<command*>* cmds,
      const std::vector<key_func*>* kfs, void* ptr);
  void process()
  {
    char str[100];
    ssize_t res = ::read(fd, str, sizeof(str));
    if (res <= 0) throw slankdev::exception("OKASHII");

    press_keys(str, res);
    refresh_prompt();
  }
  void refresh_prompt();
  void exec_command();
};
inline void shell::refresh_prompt()
{
  char lineclear[] = {slankdev::AC_ESC, '[', 2, slankdev::AC_K, '\0'};
  Printf("\r%s", lineclear);
  Printf("\r%s%s", prompt.c_str(), ibuf.c_str());

  size_t backlen = ibuf.length() - ibuf.index();
  char left [] = {slankdev::AC_ESC, '[', slankdev::AC_D, '\0'};
  for (size_t i=0; i<backlen; i++) {
    Printf("%s", left);
  }
}


struct KF_return : public key_func {
  KF_return(const void* c, size_t l) : key_func(c, l) {}
  void function(shell* sh) { sh->exec_command(); }
};
struct KF_backspace : public key_func {
  KF_backspace(const void* c, size_t l) : key_func(c, l) {}
  void function(shell* sh) { sh->ibuf.cursor_backspace(); }
};
struct KF_right : public key_func {
  KF_right(const void* c, size_t l) : key_func(c, l) {}
  void function(shell* sh) { sh->ibuf.cursor_right(); }
};
struct KF_left : public key_func {
  KF_left(const void* c, size_t l) : key_func(c, l) {}
  void function(shell* sh) { sh->ibuf.cursor_left(); }
};
struct KF_hist_search_deep : public key_func {
  KF_hist_search_deep(const void* c, size_t l) : key_func(c, l) {}
  void function(shell* sh)
  {
    sh->ibuf.clear();
    sh->ibuf.input_str(sh->history.deep_get());
    sh->refresh_prompt();
  }
};
struct KF_hist_search_shallow : public key_func {
  KF_hist_search_shallow(const void* c, size_t l) : key_func(c, l) {}
  void function(shell* sh)
  {
    sh->ibuf.clear();
    sh->ibuf.input_str(sh->history.shallow_get());
    sh->refresh_prompt();
  }
};
struct KF_cursor_top : public key_func {
  KF_cursor_top(const void* c, size_t l) : key_func(c, l) {}
  void function(shell* sh) { sh->ibuf.cursor_top(); }
};
struct KF_cursor_end : public key_func {
  KF_cursor_end(const void* c, size_t l) : key_func(c, l) {}
  void function(shell* sh) { sh->ibuf.cursor_end(); }
};
struct KF_completion : public key_func {
  KF_completion(const void* c, size_t l) : key_func(c, l) {}
  void function(shell* sh);
};


struct KF_help : public key_func {
  KF_help(const void* c, size_t l) : key_func(c, l) {}
  void function(shell* sh);
};
static inline bool endisspace(std::string str)
{
  const char* istr = str.c_str();
  return (istr[strlen(istr)-1] == ' ');
}
inline bool is_prefix(std::string str, std::string pref)
{
  auto ret = str.find(pref);
  if (ret != std::string::npos) {
    if (ret == 0) return true;
  }
  return false;
}

class vty {
  const uint16_t          port;
  const std::string       bootmsg;
  const std::string       prompt;
  int                     server_fd;
  std::vector<shell>      shells;
  std::vector<command*>   commands;
  std::vector<key_func*>  keyfuncs;

  int get_server_sock();
 public:
  void* user_ptr;

  vty(uint16_t p, const char* msg, const char* prmpt)
    : port(p), bootmsg(msg), prompt(prmpt), server_fd(get_server_sock())
  {
    add_default_keyfunctions();
    init_commands();
  }
  virtual ~vty()
  {
    for (command* c : commands) delete c;
    for (key_func* f : keyfuncs) delete f;
  }

  void install_keyfunction(key_func* kf) { keyfuncs.push_back(kf); }
  void install_command(command* cmd) { commands.push_back(cmd); }
  void init_commands();
  void add_default_keyfunctions();
  void init_default_keyfunc();
  void dispatch();
};


namespace ssn_cmd {

struct echo : public command {
  echo()
  {
    nodes.push_back(new node_fixedstring("echo", ""));
    nodes.push_back(new node_string              );
  }
  void func(shell* sh)
  {
    std::string s = nodes[1]->get();
    sh->Printf("%s\n", s.c_str());
  }
};

struct show_author : public command {
  show_author()
  {
    nodes.push_back(new node_fixedstring("show", ""));
    nodes.push_back(new node_fixedstring("author", ""));
  }
  void func(shell* sh)
  {
    sh->Printf("Hiroki SHIROKURA.\r\n");
    sh->Printf(" Twitter : @\r\n");
    sh->Printf(" Github  : \r\n");
    sh->Printf(" Facebook: hiroki.shirokura\r\n");
    sh->Printf(" E-mail  : slank.dev@gmail.com\r\n");
  }
};

struct show_version : public command {
  show_version()
  {
    nodes.push_back(new node_fixedstring("show", ""));
    nodes.push_back(new node_fixedstring("version", ""));
  }
  void func(shell* sh)
  {
    sh->Printf("Susanow 0.0.0\r\n");
    sh->Printf("Copyright 2017-2020 Hiroki SHIROKURA.\r\n");
  }
};

struct show : public command {
  show()
  {
    nodes.push_back(new node_fixedstring("show", ""));
  }
  void func(shell* sh) { sh->Printf("show\r\n"); }
};

struct quit : public command {
  quit() { nodes.push_back(new node_fixedstring("quit", "")); }
  void func(shell* sh) { sh->close(); }
};

struct clear : public command {
  clear() { nodes.push_back(new node_fixedstring("clear", "")); }
  void func(shell* sh) { sh->Printf("\033[2J\r\n"); }
};

struct list : public command {
  list() { nodes.push_back(new node_fixedstring("list", "")); }
  void func(shell* sh)
  {
    const std::vector<command*>& commands = *sh->commands;
    for (command* cmd : commands) {
      std::string s = "";
      for (node* nd : cmd->nodes) {
        s += nd->to_string();
        s += " ";
      }
      sh->Printf("  %s\r\n", s.c_str());
    }
  }
};

} /* namespace ssn_cmd */

void ssn_vty_thread(void*);
