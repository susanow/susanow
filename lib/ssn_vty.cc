
#include <susanow.h>
#include <ssn_sys.h>
#include <ssn_vty.h>

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
  int get_fd() const { return fd; }

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
  }
  virtual ~vty()
  {
    for (command* c : commands) delete c;
    for (key_func* f : keyfuncs) delete f;
  }

  void install_keyfunction(key_func* kf) { keyfuncs.push_back(kf); }
  void install_command(command* cmd) { commands.push_back(cmd); }
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

struct show_cpu : public command {
  show_cpu()
  {
    nodes.push_back(new node_fixedstring("show", ""));
    nodes.push_back(new node_fixedstring("cpu", ""));
  }
  void func(shell* sh)
  {
    sh->Printf("show cpu\r\n");
    int fd = sh->get_fd();
    FILE* fp = fdopen(sh->get_fd(), "w");
    ssn_cpu_debug_dump(fp);
    fflush(fp);
  }
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

bool command::match(const std::string& str)
{
  std::vector<std::string> list = slankdev::split(str, ' ');
  if (list.size() != nodes.size()) {
    return false;
  }

  for (size_t i=0; i<list.size(); i++) {
    if (nodes[i]->match(list[i]) == false) {
      return false;
    }
  }
  return true;
}

void shell::press_keys(const void* d, size_t l)
{
  const uint8_t* p = reinterpret_cast<const uint8_t*>(d);
  if (l == 0) throw slankdev::exception("empty data received");

  for (key_func* kf : *keyfuncs) {
    if (kf->match(p, l)) {
      kf->function(this);
      return ;
    }
  }

  if (l > 1) {
    return ;
  }
  ibuf.input_char(p[0]);
}


shell::shell(
    int d,
    const char* bootmsg,
    const char* prmpt,
    const std::vector<command*>* cmds,
    const std::vector<key_func*>* kfs,
    void* ptr
    ) :
  prompt(prmpt),
  fd(d),
  closed(false),
  commands(cmds),
  keyfuncs(kfs),
  user_ptr(ptr)
{
  Printf(bootmsg);
  refresh_prompt();
}


void shell::exec_command()
{
  Printf("\r\n");
  if (!ibuf.empty()) {
    history.add(ibuf.to_string());
    for (size_t i=0; i<commands->size(); i++) {
      if (commands->at(i)->match(ibuf.c_str())) {
        commands->at(i)->func(this);
        ibuf.clear();
        history.clean();
        Printf("\r%s%s", prompt.c_str(), ibuf.c_str());
        refresh_prompt();
        return ;
      }
    }
    Printf("command not found: \"%s\"\r\n", ibuf.c_str());
  }
  history.clean();
  ibuf.clear();
  Printf("\r%s%s", prompt.c_str(), ibuf.c_str());
  refresh_prompt();
}



void KF_help::function(shell* sh)
{
  const std::vector<command*>& commands = *sh->commands;
  std::vector<std::string> list = slankdev::split(sh->ibuf.c_str(), ' ');
  if (endisspace(sh->ibuf.c_str()) || list.empty()) list.push_back("");

  sh->Printf("\r\n");
  std::vector<std::string> match;
  for (command* cmd : commands) {

    for (size_t i=0; i<list.size(); i++) {

      if (i == cmd->nodes.size()) {

        if (list[i] == "") {
          sh->Printf("  <CR>\r\n");
        } else {
          sh->Printf("  %% There is no matched command.\r\n");
        }
        return ;

      } else {

        if (!cmd->nodes[i]->match_prefix(list[i])) {
          break;
        } else {
          if (i+1==list.size()) {
            std::string s = slankdev::fs("%-10s:   %-20s",
                cmd->nodes[i]->to_string().c_str(),
                cmd->nodes[i]->msg().c_str()
                );
            match.push_back(s);
          } else {
            ;
          }
        }

      }

    }

  }

  std::sort(match.begin(), match.end());
  match.erase(std::unique(match.begin(), match.end()), match.end());
  for (std::string& s : match) {
    sh->Printf("  %s \r\n", s.c_str());
  }

}


void KF_completion::function(shell* sh)
{
  const std::vector<command*>& commands = *sh->commands;
  std::vector<std::string> list = slankdev::split(sh->ibuf.c_str(), ' ');
  if (endisspace(sh->ibuf.c_str()) || list.empty()) list.push_back("");

  sh->Printf("\r\n");
  std::vector<std::string> match;
  for (command* cmd : commands) {

    for (size_t i=0; i<list.size(); i++) {

      if (i == cmd->nodes.size()) {

        if (list[i] == "") {
          sh->Printf("  <CR>\r\n");
        } else {
          sh->Printf("  %% There is no matched command.\r\n");
        }
        return ;

      } else {

        if (!cmd->nodes[i]->match_prefix(list[i])) {
          break;
        } else {
          if (i+1==list.size()) {
            std::string s = cmd->nodes[i]->to_string();
            match.push_back(s);
          } else {
            ;
          }
        }

      }

    }

  }

  std::sort(match.begin(), match.end());
  match.erase(std::unique(match.begin(), match.end()), match.end());

  if (match.size() == 1) {
    std::string s;
    for (size_t i=0; i<list.size(); i++) {
      if (i == list.size()-1) {
        s += match[0];
      } else {
        s += list[i];
      }
      s += " ";
    }
    sh->ibuf.clear();
    sh->ibuf.input_str(s);
  } else {
    for (std::string& s : match) {
      sh->Printf("  %s ", s.c_str());
    }
    sh->Printf("\r\n");
  }
}




int vty::get_server_sock()
{
  slankdev::socketfd server_sock;
  server_sock.noclose_in_destruct = true;
  server_sock.socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  int option = 1;
  server_sock.setsockopt(SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option));
  struct sockaddr_in addr;
  addr.sin_family = AF_INET;
  addr.sin_port   = htons(port);
  addr.sin_addr.s_addr = INADDR_ANY;
  server_sock.bind((sockaddr*)&addr, sizeof(addr));
  server_sock.listen(5);
  return server_sock.get_fd();
}

void vty::dispatch()
{
  struct Pollfd : public pollfd {
    Pollfd(int ifd, short ievents)
    {
      fd = ifd;
      events = ievents;
    }
  };
  std::vector<struct Pollfd> fds;
  fds.push_back(Pollfd(server_fd, POLLIN));
  for (const shell& sh : shells) fds.emplace_back(Pollfd(sh.fd, POLLIN));

  if (slankdev::poll(fds.data(), fds.size(), 1000)) {
    if (fds[0].revents & POLLIN) {
      /*
       * Server Accept Process
       */
      struct sockaddr_in client;
      socklen_t client_len = sizeof(client);
      int fd = accept(fds[0].fd, (sockaddr*)&client, &client_len);

      slankdev::socketfd client_sock(fd);
      uint32_t on = 1;
      client_sock.setsockopt(IPPROTO_TCP, TCP_NODELAY, &on, sizeof(on));
      int flags = client_sock.fcntl(F_GETFL);
      client_sock.fcntl(F_SETFL, (flags | O_NONBLOCK));
      client_sock.noclose_in_destruct = true;
      slankdev::vty_will_echo (fd);
      slankdev::vty_will_suppress_go_ahead (fd);
      slankdev::vty_dont_linemode (fd);
      slankdev::vty_do_window_size (fd);

      shells.push_back(
          shell(
            fd,
            bootmsg.c_str(),
            prompt.c_str(),
            &commands,
            &keyfuncs,
            user_ptr
            )
          );
    }

    /*
     * Client Read Process
     */
    for (size_t i=1; i<fds.size(); i++) {
      if (fds[i].revents & POLLIN) {
        shells[i-1].process();
        if (shells[i-1].closed) {
          close(fds[i].fd);
          shells.erase(shells.begin() + i);
          continue;
        }
      }
    }
  }
}

void vty::add_default_keyfunctions()
{
  using namespace slankdev;

  uint8_t up   [] = {AC_ESC, '[', AC_A};
  uint8_t down [] = {AC_ESC, '[', AC_B};
  uint8_t right[] = {AC_ESC, '[', AC_C};
  uint8_t left [] = {AC_ESC, '[', AC_D};
  install_keyfunction(new KF_hist_search_deep   (up   , sizeof(up   )));
  install_keyfunction(new KF_hist_search_shallow(down , sizeof(down )));
  install_keyfunction(new KF_right(right, sizeof(right)));
  install_keyfunction(new KF_left (left , sizeof(left )));

  uint8_t ctrlP[] = {AC_Ctrl_P};
  uint8_t ctrlN[] = {AC_Ctrl_N};
  uint8_t ctrlF[] = {AC_Ctrl_F};
  uint8_t ctrlB[] = {AC_Ctrl_B};
  install_keyfunction(new KF_hist_search_deep   (ctrlP, sizeof(ctrlP)));
  install_keyfunction(new KF_hist_search_shallow(ctrlN, sizeof(ctrlN)));
  install_keyfunction(new KF_right(ctrlF, sizeof(ctrlF)));
  install_keyfunction(new KF_left (ctrlB, sizeof(ctrlB)));

  uint8_t ctrlA[] = {AC_Ctrl_A};
  uint8_t ctrlE[] = {AC_Ctrl_E};
  install_keyfunction(new KF_cursor_top(ctrlA, sizeof(ctrlA)));
  install_keyfunction(new KF_cursor_end(ctrlE, sizeof(ctrlE)));

  uint8_t question[] = {'?'};
  install_keyfunction(new KF_help(question, sizeof(question)));

  uint8_t tab[] = {'\t'};
  install_keyfunction(new KF_completion(tab, sizeof(tab)));

  uint8_t ret[] = {'\r', '\0'};
  install_keyfunction(new KF_return  (ret, sizeof(ret)));
  uint8_t CtrlJ[] = {AC_Ctrl_J};
  install_keyfunction(new KF_return  (CtrlJ, sizeof(CtrlJ)));

  uint8_t backspace[] = {0x7f};
  install_keyfunction(new KF_backspace  (backspace, sizeof(backspace)));
}



struct slank : public command {
  slank() { nodes.push_back(new node_fixedstring("slank", "")); }
  void func(shell* sh)
  {
    sh->Printf("slankdev\r\n");
  }
};

ssn_vty::ssn_vty(uint32_t addr, uint16_t port)
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
  v = new vty(port, str, "ssn> ");
}
ssn_vty::~ssn_vty() { delete v; }


bool ssn_vty_poll_thread_running;
void ssn_vty_poll_thread(void* arg)
{
  ssn_vty* Vty = reinterpret_cast<ssn_vty*>(arg);
  Vty->v->install_command(new slank);
  Vty->v->install_command(new ssn_cmd::quit        );
  Vty->v->install_command(new ssn_cmd::clear       );
  Vty->v->install_command(new ssn_cmd::echo        );
  Vty->v->install_command(new ssn_cmd::list        );
  Vty->v->install_command(new ssn_cmd::show_cpu    );
  Vty->v->install_command(new ssn_cmd::show_author );
  Vty->v->install_command(new ssn_cmd::show_version);

  ssn_vty_poll_thread_running = true;
  while (ssn_vty_poll_thread_running) {
    Vty->v->dispatch();
    ssn_sleep(1);
  }
  ssn_log(SSN_LOG_DEBUG, "ret ssn_vty_poll_thread\n");
}
void ssn_vty_poll_thread_stop() { ssn_vty_poll_thread_running = false; }


