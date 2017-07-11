
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
 * @file   ssn_vty.cc
 * @brief  vty implementation
 * @author Hiroki SHIROKURA
 * @date   2017.7.12
 */



#include <stdint.h>
#include <stddef.h>
#include <ssn_vty.h>
#include <vty_server.h>

/*
 *======================================
 */

/* echo */
vty_cmd_match echo_mt()
{
  vty_cmd_match m;
  m.nodes.push_back(new node_fixedstring("echo", ""));
  m.nodes.push_back(new node_string              );
  return m;
}
void echo_f(vty_cmd_match* m, vty_client* sh, void* arg)
{
  std::string s = m->nodes[1]->get();
  sh->Printf("%s\n", s.c_str());
}

/* show_author */
vty_cmd_match show_author_mt()
{
  vty_cmd_match m;
  m.nodes.push_back(new node_fixedstring("show", ""));
  m.nodes.push_back(new node_fixedstring("author", ""));
  return m;
}
void show_author_f(vty_cmd_match* m, vty_client* sh, void* arg)
{
  sh->Printf("Hiroki SHIROKURA.\r\n");
  sh->Printf(" Twitter : @\r\n");
  sh->Printf(" Github  : \r\n");
  sh->Printf(" Facebook: hiroki.shirokura\r\n");
  sh->Printf(" E-mail  : slank.dev@gmail.com\r\n");
}


/* show_version */
vty_cmd_match show_version_mt()
{
  vty_cmd_match m;
  m.nodes.push_back(new node_fixedstring("show", ""));
  m.nodes.push_back(new node_fixedstring("version", ""));
  return m;
}
void show_version_f(vty_cmd_match* m, vty_client* sh, void* arg)
{
  sh->Printf("Susanow 0.0.0\r\n");
  sh->Printf("Copyright 2017-2020 Hiroki SHIROKURA.\r\n");
}

/* quit */
vty_cmd_match quit_mt()
{
  vty_cmd_match m;
  m.nodes.push_back(new node_fixedstring("quit", ""));
  return m;
}
void quit_f(vty_cmd_match* m, vty_client* sh, void* arg)
{
  sh->close();
}

/* clear */
vty_cmd_match clear_mt()
{
  vty_cmd_match m;
  m.nodes.push_back(new node_fixedstring("clear", ""));
  return m;
}
void clear_f(vty_cmd_match* m, vty_client* sh, void* arg)
{
  sh->Printf("\033[2J\r\n");
}

/* list */
vty_cmd_match list_mt()
{
  vty_cmd_match m;
  m.nodes.push_back(new node_fixedstring("list", ""));
  return m;
}
void list_f(vty_cmd_match* m, vty_client* sh, void* arg)
{
  const std::vector<vty_cmd*>& commands = *sh->commands;
  for (vty_cmd* cmd : commands) {
    std::string s = "";
    for (node* nd : cmd->match.nodes) {
      s += nd->to_string();
      s += " ";
    }
    sh->Printf("  %s\r\n", s.c_str());
  }
}

/* slank */
vty_cmd_match slank_mt()
{
  vty_cmd_match m;
  m.nodes.push_back(new node_fixedstring("slank", ""));
  return m;
}
void slank_f(vty_cmd_match* m, vty_client* sh, void* arg)
{
  size_t* n = reinterpret_cast<size_t*>(arg);
  sh->Printf("slankdev called %zd times\r\n", (*n)++);
}


/*
 *======================================
 */


size_t slank_f_call_cnt = 0;
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
  vty_ = new vty_server(addr, port, str, "ssn> ");
  vty_->install_command(slank_mt       (), slank_f       , &slank_f_call_cnt);
  vty_->install_command(quit_mt        (), quit_f        , nullptr);
  vty_->install_command(clear_mt       (), clear_f       , nullptr);
  vty_->install_command(echo_mt        (), echo_f        , nullptr);
  vty_->install_command(list_mt        (), list_f        , nullptr);
  vty_->install_command(show_author_mt (), show_author_f , nullptr);
  vty_->install_command(show_version_mt(), show_version_f, nullptr);
}
ssn_vty::~ssn_vty() { delete vty_; }
void ssn_vty::poll_dispatch() { vty_->poll_dispatch(); }
void ssn_vty::install_command(vty_cmd_match m, vty_cmdcallback_t f, void* arg)
{ vty_->install_command(m, f, arg); }

bool _ssn_vty_poll_thread_running;
void ssn_vty_poll_thread_stop() { _ssn_vty_poll_thread_running = false; }
void ssn_vty_poll_thread(void* arg)
{
  ssn_vty* v = reinterpret_cast<ssn_vty*>(arg);
  _ssn_vty_poll_thread_running = true;
  while (_ssn_vty_poll_thread_running) {
    v->poll_dispatch();
  }
  printf("Finish %s\n", __func__);
}


