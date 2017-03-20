



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





class lthread_schedule_kill : public slankdev::command {
 public:
  lthread_schedule_kill() {
    nodes.push_back(new slankdev::node_fixedstring("lthread", "Slow Thread running on lthreadsched"));
    nodes.push_back(new slankdev::node_fixedstring("schedule", "lthread scheduler"));
    nodes.push_back(new slankdev::node_fixedstring("kill", "kill lthread scheduler"));
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
    nodes.push_back(new slankdev::node_fixedstring("lthread", "Slow Thread running on lthreadsched"));
    nodes.push_back(new slankdev::node_fixedstring("schedule", "lthread scheduler"));
    nodes.push_back(new slankdev::node_fixedstring("run", "start lthread scheduler"));
  }
  void func(slankdev::shell* sh) {
    System* sys = get_sys(sh);
    sh->Printf("  start lthread scheduler\r\n");
    sys->lthread_sched_run();
  }
};



class launch_lthread : public slankdev::command {
public:
    launch_lthread()
    {
        nodes.push_back(new slankdev::node_fixedstring("launch", "Thread Launch"));
        nodes.push_back(new slankdev::node_fixedstring("lthread", "Fast Thread running on lcore"));
        nodes.push_back(new slankdev::node_string);
    }
    void func(slankdev::shell* sh)
    {
        System* sys = get_sys(sh);
        sh->Printf("  Launch \"%s\" \r\n", nodes[2]->get().c_str());

        Lthread* thread = sys->lthreadpool.find_name2ptr(nodes[2]->get());
        if (thread) {
            sys->launch_Lthread(thread);
        } else {
            sh->Printf("  Thread Not Found \r\n");
        }
    }
};

class launch_fthread : public slankdev::command {
public:
    launch_fthread()
    {
        nodes.push_back(new slankdev::node_fixedstring("launch", "Thread Launch"));
        nodes.push_back(new slankdev::node_fixedstring("fthread", "Fast Thread running on lcore"));
        nodes.push_back(new slankdev::node_string);
    }
    void func(slankdev::shell* sh)
    {
        System* sys = get_sys(sh);
        sh->Printf("  Launch \"%s\" \r\n", nodes[2]->get().c_str());

        Fthread* thread = sys->fthreadpool.find_name2ptr(nodes[2]->get());
        if (thread) {
            sys->launch_Fthread(thread);
        } else {
            sh->Printf("  Thread Not Found \r\n");
        }
    }
};

class kill_fthread : public slankdev::command {
public:
    kill_fthread()
    {
        nodes.push_back(new slankdev::node_fixedstring("kill", "Thread kill"));
        nodes.push_back(new slankdev::node_fixedstring("fthread", "Fast Thread running on lcore"));
        nodes.push_back(new slankdev::node_string);
    }
    void func(slankdev::shell* sh)
    {
        System* sys = get_sys(sh);
        sh->Printf("  Kill \"%s\" \r\n", nodes[2]->get().c_str());

        Fthread* thread = sys->fthreadpool.find_name2ptr(nodes[2]->get());
        if (thread) {
            sys->kill_Fthread(thread);
        } else {
            sh->Printf("  Thread Not Found \r\n");
        }
    }
};

class kill_lthread : public slankdev::command {
public:
    kill_lthread()
    {
        nodes.push_back(new slankdev::node_fixedstring("kill", "Thread kill"));
        nodes.push_back(new slankdev::node_fixedstring("lthread", "Slow Thread"));
        nodes.push_back(new slankdev::node_string);
    }
    void func(slankdev::shell* sh)
    {
        System* sys = get_sys(sh);
        sh->Printf("  Kill \"%s\" \r\n", nodes[2]->get().c_str());

        Lthread* thread = sys->lthreadpool.find_name2ptr(nodes[2]->get());
        if (thread) {
            sys->kill_Lthread(thread);
        } else {
            sh->Printf("  Thread Not Found \r\n");
        }
    }
};



struct find_fthread : public slankdev::command {
    find_fthread()
    {
        nodes.push_back(new slankdev::node_fixedstring("find", "find Thread"));
        nodes.push_back(new slankdev::node_fixedstring("fthread", "Fast Thread running on lcore"));
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

struct find_lthread : public slankdev::command {
    find_lthread()
    {
        nodes.push_back(new slankdev::node_fixedstring("find", "find Thread"));
        nodes.push_back(new slankdev::node_fixedstring("lthread", "Slow Thread"));
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


