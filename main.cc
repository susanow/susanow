

#include <stdio.h>
#include <string>
#include <ssnlib_sys.h>
#include <ssnlib_thread.h>
#include <ssnlib_log.h>
#include <slankdev/system.h>
#include <slankdev/filelogger.h>
#include <slankdev/vty.h>


class thread0 : public ssnlib::Thread {
    bool runnning;
public:
    thread0() : Thread("thread0"), runnning(false) {}
    void impl()
    {
        runnning = true;
        while (runnning) {
            printf("thread0 \n");
            sleep(1);
        }
    }
    bool kill() { runnning = false; return true; }

};
class thread1 : public ssnlib::Thread {
    bool runnning;
public:
    thread1() : Thread("thread1"), runnning(false) {}
    void impl()
    {
        runnning = true;
        while (runnning) {
            printf("thread1 \n");
            sleep(1);
        }
    }
    bool kill() { runnning = false; return true; }

};
class vty : public ssnlib::Thread {
    struct cmd_test : public slankdev::vty::cmd_node {
        cmd_test() : cmd_node("test") {}
        void function(slankdev::vty::shell* sh) { sh->Printf("test\r\n"); }
    };
    slankdev::vty vty_;
public:
    vty() : Thread("vty"), vty_(9999, "DFDDDFDDD\r\n")
    {
        vty_.add_command(new cmd_test);
    }
    void impl()
    {
        vty_.dispatch();
    }
    bool kill() { vty_.halt(); return true; }
};




std::string slankdev::filelogger::path = "syslog.out";
int main(int argc, char** argv)
{
    using namespace ssnlib;

    System sys(argc, argv);
    sys.threads.push_back(new thread0);
    sys.threads.push_back(new thread1);
    sys.threads.push_back(new vty);
    sys.dispatch();
    kernel_log("\n\nmain() FINISH\n\n\n");
}



