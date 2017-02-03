
#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <string>
#include <memory>
#include <readline/readline.h>
#include <readline/history.h>

#include <ssnlib_sys.h>
#include <ssnlib_thread.h>
#include <ssnlib_cmd.h>


static inline char* Readline(const char* p)
{
    char* line = readline(p);
    add_history(line);
    return line;
}

namespace ssnlib {

class Shell : public ssnlib::ssn_thread {
    std::vector<std::unique_ptr<Command>> cmds;
public:
    void help()
    {
        printf("Commands: \n");
        for (const auto& c : cmds) {
            printf("  %s \n", c->name.c_str());
        }
    }
    bool kill() { return false; }
    void add_cmd(ssnlib::Command* newcmd)
    {
        for (auto& cmd : cmds) {
            if (cmd->name == newcmd->name)
                throw slankdev::exception("command name is already registred");
        }
        cmds.push_back(std::unique_ptr<Command>(newcmd));
    }
    void exe_cmd(const char* cmd_str)
    {
        if (strlen(cmd_str) == 0) return;
        std::vector<std::string> args = slankdev::split(cmd_str, ' ');
        if (args[0] == "help") {
            help();
        } else {
            for (auto& cmd : cmds) {
                if (cmd->name == args[0]) {
                    (*cmd)(args);
                    return;
                }
            }
            printf("SUSH: command not found: %s\n", args[0].c_str());
        }
    }
    void operator()()
    {
        const char* prmpt = "SUSANOO$ ";
        while (char* line = Readline(prmpt)) {
            exe_cmd(line);
            free(line);
        }
        return;
    }
};



} /* namespace ssnlib */

