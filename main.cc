

#include <stdio.h>
#include <ssnlib_sys.h>
#include <ssnlib_thread.h>
#include <slankdev/system.h>
#include <slankdev/filelogger.h>
#include <slankdev/util.h>
#include <string>


std::string slankdev::filelogger::path = "syslog.out";
int main(int argc, char** argv)
{
    UNUSED(argc);
    UNUSED(argv);
    using namespace ssnlib;

    System sys(argc, argv);
    kernel_log("\n\nmain() FINISH\n\n\n");
}

