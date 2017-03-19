

#include <string>
#include <slankdev/exception.h>
#include <ssnlib_log.h>
#include <slankdev/vty.h>
#include <lthread_api.h>
#include <ssnlib_thread.h>



volatile bool force_quit;
namespace ssnlib {

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


void lthread_sched::kill() { ::force_quit = true; }


void lthread_sched::impl()
{
    size_t nb_threads = slowthreads.size();
    printf("Lthread: Launch %zd threads...\n", nb_threads);
    struct lthread *lt[nb_threads];
    for (size_t i=0; i<nb_threads; i++) {
        lthread_create (
                &lt[i], -1,
                lthread_sched::lthread_start,
                slowthreads.get_thread(i)
        );
    }
    lthread_run();
    printf("Lthread finished \n");
}




} /* namespace ssnlib */



