



/*-
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
 * @file commands.h
 * @author slankdev
 */


#pragma once
#include <slankdev/vty.h>
#include <slankdev/dpdk_struct.h>
#include <ssnlib_thread.h>
#include <ssnlib_misc.h>




class echo : public slankdev::command {
 public:
  echo()
  {
    nodes.push_back(new slankdev::node_fixedstring("echo", "echoing user message"));
    nodes.push_back(new slankdev::node_string              );
  }
  void func(slankdev::shell* sh)
  {
    std::string s = nodes[1]->get();
    sh->Printf("%s\n", s.c_str());
  }
};

class quit : public slankdev::command {
 public:
  quit() { nodes.push_back(new slankdev::node_fixedstring("quit", "quit system")); }
  void func(slankdev::shell* sh) { sh->close(); }
};

class clear : public slankdev::command {
 public:
  clear() { nodes.push_back(new slankdev::node_fixedstring("clear", "clear screen")); }
  void func(slankdev::shell* sh) { sh->Printf("\033[2J\r\n"); }
};

class list : public slankdev::command {
 public:
  list() { nodes.push_back(new slankdev::node_fixedstring("list", "list avalable commands")); }
  void func(slankdev::shell* sh)
  {
    const std::vector<slankdev::command*>& commands = *sh->commands;
    for (slankdev::command* cmd : commands) {
      std::string s = "";
      for (slankdev::node* nd : cmd->nodes) {
        s += nd->to_string();
        s += " ";
      }
      sh->Printf("  %s\r\n", s.c_str());
    }
  }
};

class show_author : public slankdev::command {
 public:
  show_author()
  {
    nodes.push_back(new slankdev::node_fixedstring("show", "show infos"));
    nodes.push_back(new slankdev::node_fixedstring("author", "Author info"));
  }
  void func(slankdev::shell* sh)
  {
    sh->Printf("Hiroki SHIROKURA.\r\n");
    sh->Printf(" Twitter : @\r\n");
    sh->Printf(" Github  : \r\n");
    sh->Printf(" Facebook: hiroki.shirokura\r\n");
    sh->Printf(" E-mail  : slank.dev@gmail.com\r\n");
  }
};

class show_version : public slankdev::command {
 public:
  show_version()
  {
    nodes.push_back(new slankdev::node_fixedstring("show", "show infos"));
    nodes.push_back(new slankdev::node_fixedstring("version", "Print version"));
  }
  void func(slankdev::shell* sh)
  {
    sh->Printf("Susanow 0.0.0\r\n");
    sh->Printf("Copyright 2017-2020 Hiroki SHIROKURA.\r\n");
  }
};

class show_thread_info : public slankdev::command {
  std::string get_launch_state(System* sys, const Fthread* thread)
  {
    const size_t nb_cpus = sys->cpus.size();
    for (size_t i=1; i<nb_cpus; i++) {
      if (sys->cpus[i].thread == thread) {
        return "lcore" + std::to_string(i);
      }
    }
    return "none";
  }
 public:
  show_thread_info()
  {
    nodes.push_back(new slankdev::node_fixedstring("show", "show infos"));
    nodes.push_back(new slankdev::node_fixedstring("thread-info", "thread information"));
  }
  void func(slankdev::shell* sh);
};

class show_port : public slankdev::command {
 public:
  show_port()
  {
    nodes.push_back(new slankdev::node_fixedstring("show", "show infos"));
    nodes.push_back(new slankdev::node_fixedstring("port", "port info"));
  }
  void func(slankdev::shell* sh)
  {
    System* sys = get_sys(sh);
    for (size_t i=0; i<sys->ports.size(); i++) {
      auto& port = sys->ports.at(i);
      sh->Printf("  Port id      : %zd\r\n", i);
      sh->Printf("  Link speed   : %u Mbps\r\n",
          port.link.raw.link_speed);
      sh->Printf("  Link duplex  : %s\r\n",
          port.link.raw.link_duplex==0?"Half":"Full");
      sh->Printf("  Link autoneg : %s\r\n",
          port.link.raw.link_autoneg==0?"Fixed":"Autoneg");
      sh->Printf("  Link status  : %s\r\n",
          port.link.raw.link_status==0?"Down":"Up");
      sh->Printf("  NB Tx Queue  : %zd\r\n", port.nb_rx_rings);
      sh->Printf("  Tx Queue size: %zd\r\n", port.rx_ring_size);
      sh->Printf("  NB Rx Queue  : %zd\r\n", port.nb_tx_rings);
      sh->Printf("  Rx Queue size: %zd\r\n", port.tx_ring_size);
      sh->Printf("  Mbps Rx/Tx   : %zd/%zd\r\n",
          port.stats.rx_bps/1000000, port.stats.tx_bps/1000000);
      sh->Printf("  Kpps Rx/Tx   : %zd/%zd \r\n",
          port.stats.rx_pps/1000   , port.stats.tx_pps/1000);
      sh->Printf("\r\n");
    }
  }
};

class show_cpu : public slankdev::command {
 public:
  show_cpu()
  {
    nodes.push_back(new slankdev::node_fixedstring("show", "show infos"));
    nodes.push_back(new slankdev::node_fixedstring("cpu", "cpu infos"));
  }
  void func(slankdev::shell* sh)
  {
    sh->Printf(" %-4s %-10s %-20s %-20s\r\n", "ID", "State", "Thread", "Name");
    System* sys = get_sys(sh);
    for (size_t i=0; i<sys->cpus.size(); i++) {
      std::string state = get_cpustate(i);
      Fthread* thread = sys->cpus[i].thread;
      sh->Printf(" %-4zd %-10s %-20p %-20s\r\n", i, state.c_str(),
          thread, thread?thread->name.c_str():"none");
    }
  }
};


void show_thread_info::func(slankdev::shell* sh)
{
  sh->Printf("\r\n");
  sh->Printf(" CPU state\r\n");
  sh->Printf("   %-4s %-10s %-20s %-20s\r\n", "ID", "State", "Thread", "Name");
  System* sys = get_sys(sh);
  for (size_t i=0; i<sys->cpus.size(); i++) {
    std::string state = get_cpustate(i);
    Fthread* thread = sys->cpus[i].thread;
    sh->Printf("   %-4zd %-10s %-20p %-20s\r\n", i, state.c_str(),
        thread, thread?thread->name.c_str():"none");
  }
  sh->Printf("\r\n");


  const Fthread* thread;
  size_t nb_threads;
  std::string state;
  sh->Printf(" Fthreads\r\n");
  sh->Printf("   %-4s %-20s %-20s %-10s\r\n", "No.", "Name", "Ptr", "State");
  thread = &sys->vty;
  state = get_launch_state(sys, thread);
  sh->Printf("   %-4s %-20s %-20p %-10s\r\n",
      "N/A", thread->name.c_str(), thread, state.c_str());
  thread = &sys->ltsched;
  state = get_launch_state(sys, thread);
  sh->Printf("   %-4s %-20s %-20p %-10s\r\n",
      "N/A", thread->name.c_str(), thread, state.c_str());
  nb_threads = sys->fthreadpool.size();
  for (size_t i=0; i<nb_threads; i++) {
    thread = sys->fthreadpool.get_thread(i);
    state = get_launch_state(sys, thread);
    sh->Printf("   %-4zd %-20s %-20p %-10s\r\n",
        i,
        thread->name.c_str(),
        thread,
        state.c_str());
  }
  sh->Printf("\r\n");

  /*
   * Show Lthread infos
   */
  sh->Printf(" Lthreads\r\n");
  sh->Printf("   %-4s %-20s %-10s %-20s \r\n", "No.", "Name", "Ptr", "State");
  nb_threads = sys->lthreadpool.size();
  for (size_t i = 0; i<nb_threads; i++) {
    const Lthread* thread = sys->lthreadpool.get_thread(i);
    sh->Printf("   %-4zd %-20s %-20p %-10s\r\n",
        i,
        thread->name.c_str(),
        thread,
        thread->running?"running":"stop");
  }
  sh->Printf("\r\n");

  sh->Printf(" Tthreads\r\n");
  sh->Printf("   %-4s %-20s %-20s \r\n", "No.", "Name", "Ptr");
  nb_threads = sys->tthreadpool.size();
  for (size_t i = 0; i<nb_threads; i++) {
    const Tthread* thread = sys->tthreadpool.get_thread(i);
    sh->Printf("   %-4zd %-20s %-20p \r\n",
        i,
        thread->name.c_str(),
        thread);
  }
  sh->Printf("\r\n");
}
