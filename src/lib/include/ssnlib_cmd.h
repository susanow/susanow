
#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <string>


namespace ssnlib {


class Command {
    friend class Shell;
protected:
    const std::string name;
public:
    Command(const char* n) : name(n) {}
    virtual void operator()(const std::vector<std::string>& args) = 0;
    virtual ~Command() {}
};


// class Cmd_reboot
// class Cmd_show
//    show config
//    show route
//    show stats
//    show statistic
// class Cmd_port
//    port 0 linkdown
//    port 0 linkup
//    port 0 blink
// class Cmd_commit
// class Cmd_export
// class Cmd_inport






} /* namespace ssnlib */
