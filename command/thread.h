



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
 * @file commands.h
 * @author slankdev
 */


#pragma once
#include <slankdev/vty.h>
#include <slankdev/dpdk_struct.h>
#include <ssnlib_thread.h>
#include <ssnlib_misc.h>





inline slankdev::node* fixed_lthread()
{
  return new slankdev::node_fixedstring(
      "lthread", "Slow Thread running on lthreadsched");
}

inline slankdev::node* fixed_fthread()
{
  return new slankdev::node_fixedstring(
      "fthread", "Falst Thread runnig on Lcore");
}

inline slankdev::node* fixed_list()
{ return new slankdev::node_fixedstring("list", "List emements"); }

inline slankdev::node* fixed_find()
{ return new slankdev::node_fixedstring("find", "Find Thread"); }

inline slankdev::node* fixed_launch()
{ return new slankdev::node_fixedstring("launch", "Launch Thread"); }

inline slankdev::node* fixed_kill()
{ return new slankdev::node_fixedstring("kill", "Kill Thread"); }




/*
 * Lthread Commands
 */

#if 0
class lthread_schedule_kill : public slankdev::command {
 public:
  lthread_schedule_kill() {
    nodes.push_back(fixed_lthread());
    nodes.push_back(new slankdev::node_fixedstring("schedule", "lthread scheduler"));
    nodes.push_back(new slankdev::node_fixedstring("kill", "stop lthread scheduler"));
  }
  void func(slankdev::shell* sh) {
    System* sys = get_sys(sh);
    sh->Printf("  kill lthread scheduler\r\n");
    sys->lthread_sched_kill();
  }
};

class lthread_schedule_run : public slankdev::command {
 public:
  lthread_schedule_run() {
    nodes.push_back(fixed_lthread());
    nodes.push_back(new slankdev::node_fixedstring("schedule", "lthread scheduler"));
    nodes.push_back(new slankdev::node_fixedstring("run", "start lthread scheduler"));
  }
  void func(slankdev::shell* sh) {
    System* sys = get_sys(sh);
    sh->Printf("  start lthread scheduler\r\n");
    sys->lthread_sched_run();
  }
};
#endif


class lthread_list : public slankdev::command {
 public:
  lthread_list()
  {
    nodes.push_back(fixed_lthread());
    nodes.push_back(fixed_list());
  }
  void func(slankdev::shell* sh)
  {
    System* sys = get_sys(sh);
    sh->Printf("\r\n");
    sh->Printf("   %-4s %-20s %-10s %-20s \r\n", "No.", "Name", "Ptr", "State");
    size_t nb_threads = sys->lthreadpool.size();
    for (size_t i = 0; i<nb_threads; i++) {
      const Lthread* thread = sys->lthreadpool.get_thread(i);
      sh->Printf("   %-4zd %-20s %-20p %-10s\r\n",
          i,
          thread->name.c_str(),
          thread,
          thread->running?"running":"stop");
    }
    sh->Printf("\r\n");
  }
};

class lthread_find : public slankdev::command {
 public:
  lthread_find()
  {
    nodes.push_back(fixed_lthread());
    nodes.push_back(fixed_find());
    nodes.push_back(new slankdev::node_string);
  }
  void func(slankdev::shell* sh)
  {
    System* sys = get_sys(sh);
    sh->Printf("  Thread Name: \"%s\" \r\n", nodes[2]->get().c_str());

    Lthread* thread = sys->lthreadpool.find_name2ptr(nodes[2]->get());
    if (thread) {
      sh->Printf("  Found: %p \r\n", thread);
    } else {
      sh->Printf("  Not Found by %zd lthreads\r\n", sys->lthreadpool.size());
    }
  }
};
class lthread_launch : public slankdev::command {
 public:
  lthread_launch()
  {
    nodes.push_back(fixed_lthread());
    nodes.push_back(fixed_launch());
    nodes.push_back(new slankdev::node_string);
  }
  void func(slankdev::shell* sh)
  {
    System* sys = get_sys(sh);
    sh->Printf("  Launch \"%s\" \r\n", nodes[2]->get().c_str());

    Lthread* lthread = sys->lthreadpool.find_name2ptr(nodes[2]->get());
    if (lthread) {
      sys->lthread_launch(lthread);
    } else {
      sh->Printf("  Thread Not Found \r\n");
    }
  }
};

class lthread_kill : public slankdev::command {
 public:
  lthread_kill()
  {
    nodes.push_back(fixed_lthread());
    nodes.push_back(fixed_kill());
    nodes.push_back(new slankdev::node_string);
  }
  void func(slankdev::shell* sh)
  {
    System* sys = get_sys(sh);
    sh->Printf("  Kill \"%s\" \r\n", nodes[2]->get().c_str());

    Lthread* lthread = sys->lthreadpool.find_name2ptr(nodes[2]->get());
    if (lthread) {
      sys->lthread_kill(lthread);
    } else {
      sh->Printf("  Thread Not Found \r\n");
    }
  }
};




/*
 * Fthread Commands
 */

class fthread_find : public slankdev::command {
 public:
  fthread_find()
  {
    nodes.push_back(fixed_fthread());
    nodes.push_back(fixed_find());
    nodes.push_back(new slankdev::node_string);
  }
  void func(slankdev::shell* sh)
  {
    System* sys = get_sys(sh);
    sh->Printf("  Thread Name: \"%s\" \r\n", nodes[2]->get().c_str());

    Fthread* thread = sys->fthreadpool.find_name2ptr(nodes[2]->get());
    if (thread) {
      sh->Printf("  Found: %p \r\n", thread);
    } else {
      sh->Printf("  Not Found \r\n");
    }
  }
};


class fthread_launch : public slankdev::command {
 public:
  fthread_launch()
  {
    nodes.push_back(fixed_fthread());
    nodes.push_back(fixed_launch());
    nodes.push_back(new slankdev::node_string);
  }
  void func(slankdev::shell* sh)
  {
    System* sys = get_sys(sh);
    sh->Printf("  Launch \"%s\" \r\n", nodes[2]->get().c_str());

    Fthread* fthread = sys->fthreadpool.find_name2ptr(nodes[2]->get());
    if (fthread) {
      sys->fthread_launch(fthread);
    } else {
      sh->Printf("  Thread Not Found \r\n");
    }
  }
};

class fthread_kill : public slankdev::command {
 public:
  fthread_kill()
  {
    nodes.push_back(fixed_fthread());
    nodes.push_back(fixed_kill());
    nodes.push_back(new slankdev::node_string);
  }
  void func(slankdev::shell* sh)
  {
    System* sys = get_sys(sh);
    sh->Printf("  Kill \"%s\" \r\n", nodes[2]->get().c_str());

    Fthread* fthread = sys->fthreadpool.find_name2ptr(nodes[2]->get());
    if (fthread) {
      sys->fthread_kill(fthread);
    } else {
      sh->Printf("  Thread Not Found \r\n");
    }
  }
};



