
#include <vty.h>
#include "commands.h"


int main()
{
    slankdev::vty vty0(9999);

    vty0.add_command(new slankdev::Cmd_show);
    vty0.add_command(new slankdev::Cmd_quit);
    vty0.dispatch();
}

