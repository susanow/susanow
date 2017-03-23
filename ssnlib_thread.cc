


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
 * @file ssnlib_thread.cc
 * @author slankdev
 */


#include <ssnlib_thread.h>

#include <string>
#include <lthread_api.h>

#include <slankdev/vty.h>
#include <slankdev/exception.h>

#include <ssnlib_log.h>



int Fthread::spawner(void* arg)
{
  Fthread* thread = reinterpret_cast<Fthread*>(arg);
  uint32_t lcoreid = rte_lcore_id();
  printf("Launch thread \"%s\" to lcoreid=%u \n",
                    thread->name.c_str(), lcoreid);
  thread->impl();
  return 0;
}

Fthread::Fthread(const char* n) : name(n)
{ kernel_log("Construct thread %s\n", name.c_str()); }
Fthread::~Fthread()
{ kernel_log("Destruct thread %s \n", name.c_str()); }



Lthread::Lthread(const char* n) : running(false), name(n)
{ kernel_log("Construct lthread %s\n", name.c_str()); }
Lthread::~Lthread()
{ kernel_log("Destruct lthread %s \n", name.c_str()); }
bool Lthread::is_run() const { return running; }
void Lthread::spawner(void* arg)
{
  Lthread* thread = reinterpret_cast<Lthread*>(arg);
  printf(" launch lthread \"%s\"\n", thread->name.c_str());
  thread->running = true;
  while (thread->running) {
    thread->impl();
    lthread_yield ();
  }
  printf(" lthread \"%s\" was finish\n", thread->name.c_str());
  lthread_exit (NULL);
}


Tthread::Tthread(const char* n) : name(n)
{ kernel_log("Construct tthread %s\n", name.c_str()); }
Tthread::~Tthread()
{ kernel_log("Destruct tthread %s \n", name.c_str()); }

void Tthread::spawner(struct rte_timer *, void *arg)
{
  Tthread* tthread = reinterpret_cast<Tthread*>(arg);
  tthread->impl();
}



const char* str = "\r\n"
"Hello, this is Susanow (version 0.00.00.0).\r\n"
"Copyright 2017-2020 Hiroki SHIROKURA.\r\n"
"\r\n"
" .d8888b.                                                             \r\n"
"d88P  Y88b                                                            \r\n"
"Y88b.                                                                 \r\n"
" \"Y888b.   888  888 .d8888b   8888b.  88888b.   .d88b.  888  888  888\r\n"
"    \"Y88b. 888  888 88K          \"88b 888 \"88b d88\"\"88b 888  888  888\r\n"
"      \"888 888  888 \"Y8888b. .d888888 888  888 888  888 888  888  888 \r\n"
"Y88b  d88P Y88b 888      X88 888  888 888  888 Y88..88P Y88b 888 d88P \r\n"
" \"Y8888P\"   \"Y88888  88888P\' \"Y888888 888  888  \"Y88P\"   \"Y8888888P\"\r\n"
"\r\n";

vty_thread::vty_thread(void* userptr)
  : Fthread("vty_thread"),
  vty_(9999, str, "Susanow> ") { vty_.user_ptr = userptr; }


void lthread_sched::impl()
{
  class startlthread : public Lthread {
   public:
    startlthread() : Lthread("test") {}
    void impl() override {}
    void kill() override {}
  };

  printf("Lthread: Launch scheduler start \n");

  startlthread slth;
  struct lthread* lt;
  lthread_create(&lt, -1, Lthread::spawner, &slth);
  lts.push_back(lt);

  lthread_run();
  printf("Lthread finished \n");
}


void lthread_sched::launch_lthread(Lthread* lthread)
{
  struct lthread* lt;
  lthread_create(&lt, 1,  Lthread::spawner, lthread);
  lts.push_back(lt);
}

void lthread_sched::kill_lthread(Lthread* lthread)
{
  lthread->kill();
}

void lthread_sched::show(slankdev::shell* sh) const
{
  sh->Printf(" Lthread scheduler \r\n");
  for (size_t i=0; i<lts.size(); i++) {
    sh->Printf("  %p \r\n", lts[i]);
  }
}


