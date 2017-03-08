
#include "vty.h"
#include "command.h"
#include "keyfunction.h"


void init()
{
    shell::keyfuncs.push_back(new KF_question());
    shell::keyfuncs.push_back(new KF_return  ());
    shell::keyfuncs.push_back(new KF_delete  ());
    shell::keyfuncs.push_back(new KF_ctrl_B  ());

    shell::commands.push_back(new Cmd_halt("halt"));
    shell::commands.push_back(new Cmd_quit("quit"));
}

int main()
{
    init();
    vty vty(9999);
    vty.dispatch();
}


