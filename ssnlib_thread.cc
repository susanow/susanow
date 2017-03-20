


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



volatile bool force_quit;


static void _lthread_start(void* arg)
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

const char* str = "\r\n"
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

vty_thread::vty_thread(void* userptr)
    : Fthread("vty_thread"),
    vty_(9999, str, "Susanow> ") { vty_.user_ptr = userptr; }


void lthread_sched::kill() {
    printf("lthread_sched.kill()\n");
    force_quit = true;
}


void lthread_sched::impl()
{
    size_t nb_threads = slowthreads.size();
    printf("Lthread: Launch %zd threads...\n", nb_threads);
    struct lthread *lt[nb_threads];
    for (size_t i=0; i<nb_threads; i++) {
        lthread_create (
                &lt[i], -1,
                _lthread_start,
                slowthreads.get_thread(i)
        );
    }
    lthread_run();
    printf("Lthread finished \n");
    printf("SLANKDEVVVDDVDV Lthread finished \n");
}






