

/*-
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
 * @file command/port.h
 * @author slankdev
 */

#pragma once
#include <slankdev/vty.h>
#include <ssnlib_thread.h>
#include <ssnlib_misc.h>
#include <command/thread.h>
#include <command/imple.h>
#include <command/node.h>



class port_statistics : public slankdev::command {
 public:
  port_statistics()
  {
    nodes.push_back(fixed_port());
    nodes.push_back(new slankdev::node_fixedstring("statistics",
          "show statistics data"));
  }
  virtual void func(slankdev::shell* sh) override { _port_statistics(sh); }
};


class port_rxmode_show : public slankdev::command {
 public:
  port_rxmode_show()
  {
    nodes.push_back(fixed_port());
    nodes.push_back(new slankdev::node_fixedstring("rxmode", ""));
    nodes.push_back(fixed_show());
  }
  virtual void func(slankdev::shell* sh) override { _port_rxmode_show(sh); }
};



class port_rss : public slankdev::command {
 public:
  port_rss()
  {
    nodes.push_back(fixed_port());
    nodes.push_back(new slankdev::node_fixedstring("rss", ""));
  }
  virtual void func(slankdev::shell* sh) override { _port_rss(sh); }
};



class port_dev_start : public slankdev::command {
 public:
  port_dev_start()
  {
    nodes.push_back(fixed_port());
    nodes.push_back(fixed_dev());
    nodes.push_back(new slankdev::node_fixedstring("start", ""));
  }
  virtual void func(slankdev::shell* sh) override
  {
    System* sys = get_sys(sh);
    size_t nb_ports = sys->ports.size();
    for (size_t i=0; i<nb_ports; i++)
      sys->ports[i].devstart();
  }
};
class port_dev_stop : public slankdev::command {
 public:
  port_dev_stop()
  {
    nodes.push_back(fixed_port());
    nodes.push_back(fixed_dev());
    nodes.push_back(new slankdev::node_fixedstring("stop", ""));
  }
  virtual void func(slankdev::shell* sh) override
  {
    System* sys = get_sys(sh);
    size_t nb_ports = sys->ports.size();
    for (size_t i=0; i<nb_ports; i++)
      sys->ports[i].devstop();
  }
};

class port_link_down : public slankdev::command {
 public:
  port_link_down()
  {
    nodes.push_back(fixed_port());
    nodes.push_back(fixed_link());
    nodes.push_back(new slankdev::node_fixedstring("down", ""));
  }
  virtual void func(slankdev::shell* sh) override
  {
    System* sys = get_sys(sh);
    size_t nb_ports = sys->ports.size();
    for (size_t i=0; i<nb_ports; i++)
      sys->ports[i].linkdown();
  }
};

class port_link_up : public slankdev::command {
 public:
  port_link_up()
  {
    nodes.push_back(fixed_port());
    nodes.push_back(fixed_link());
    nodes.push_back(new slankdev::node_fixedstring("up", ""));
  }
  virtual void func(slankdev::shell* sh) override
  {
    System* sys = get_sys(sh);
    size_t nb_ports = sys->ports.size();
    for (size_t i=0; i<nb_ports; i++)
      sys->ports[i].linkup();
  }
};


class port_set_nbq : public slankdev::command {
 public:
  port_set_nbq()
  {
    nodes.push_back(fixed_port());
    nodes.push_back(new slankdev::node_fixedstring("set", ""));
    nodes.push_back(new slankdev::node_fixedstring("nbq", ""));
    nodes.push_back(new slankdev::node_string              );
  }
  virtual void func(slankdev::shell* sh) override
  {
    std::string s = nodes[3]->get();
    sh->Printf("set nb rings %s \r\n", s.c_str());
    try {
      size_t n = std::stoi(s);

      System* sys = get_sys(sh);
      size_t nb_ports = sys->ports.size();
      for (size_t i=0; i<nb_ports; i++) {
        sys->ports[i].nb_rx_rings = n;
        sys->ports[i].nb_tx_rings = n;
      }
    } catch (std::exception& e) {
      sh->Printf("Invalid Syntax\r\n");
    }
  }
};



class port_configure : public slankdev::command {
 public:
  port_configure()
  {
    nodes.push_back(fixed_port());
    nodes.push_back(new slankdev::node_fixedstring("configure", ""));
  }
  virtual void func(slankdev::shell* sh) override
  {
    System* sys = get_sys(sh);
    size_t nb_ports = sys->ports.size();
    for (size_t i=0; i<nb_ports; i++) {
      sys->ports[i].init();
    }
  }
};



