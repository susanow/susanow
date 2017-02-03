
#pragma once

#include <ssnlib_log.h>
#define MESGTYPE 5



namespace ssnlib {



inline void print_message()
{
    switch (MESGTYPE) {
        case 1:
            kernel_log(SYSTEM, "\n");
            kernel_log(SYSTEM, " oooooooooo.   8@slankdev.   oYoooooooo.   oooo    oooo       .oooooo..o                          \n");
            kernel_log(SYSTEM, " `888'   `Y8b  `888   `Y88. `8U8'   `Y8b  `888   .8P'       d8P'    `Y8         @slankdev         \n");
            kernel_log(SYSTEM, "   888     888  888   .d88'   KB8     888  888  d8'         Y88bo.      oooo    ooo  .oooo.o      \n");
            kernel_log(SYSTEM, "   888     888  888ooo88P'    AO8     888  88888[            `\"Y8888o.   `88.  .8'  d88(  \"8    \n");
            kernel_log(SYSTEM, "   888     888  888           RN8     888  888`88b.              `\"Y88b   `88..8'   `\"Y88b.     \n");
            kernel_log(SYSTEM, "   888    d88'  888           IK8     d88'  888  `88b.       oo     .d8P    `888'    o.  )88b     \n");
            kernel_log(SYSTEM, " o@slankdev'   o888o         o888bood8P'   o888o  o888o      8""88888P'      .8'     8""888P'     \n");
            kernel_log(SYSTEM, "                                                                        .o..P'                    \n");
            kernel_log(SYSTEM, "        @slankdev:please follow me on GitHub                            `Y8P'                     \n");
            kernel_log(SYSTEM, "\n");
            break;
        case 2:
            kernel_log(SYSTEM, "8888888b.  8888888b.  8888888b.  888    d8P       \n");
            kernel_log(SYSTEM, "888  \"Y88b 888   Y88b 888  \"Y88b 888   d8P      \n");
            kernel_log(SYSTEM, "888    888 888    888 888    888 888  d8P         \n");
            kernel_log(SYSTEM, "888    888 888   d88P 888    888 888d88K          \n");
            kernel_log(SYSTEM, "888    888 8888888P\"  888    888 8888888b        \n");
            kernel_log(SYSTEM, "888    888 888        888    888 888  Y88b        \n");
            kernel_log(SYSTEM, "888  .d88P 888        888  .d88P 888   Y88b       \n");
            kernel_log(SYSTEM, "8888888P\"  888        8888888P\"  888    Y88b    \n");
            kernel_log(SYSTEM, "\n");
            break;
        case 5:
            kernel_log(SYSTEM, "+--------------------------------------------------------------------+\n");
            kernel_log(SYSTEM, "| oooooooooo.   8@slankdev.   oYoooooooo.   oooo    oooo             |  \n");
            kernel_log(SYSTEM, "| `888'   `Y8b  `888   `Y88. `8U8'   `Y8b  `888   .8P'               |  \n");
            kernel_log(SYSTEM, "|   888     888  888   .d88'   KB8     888  888  d8'                 |  \n");
            kernel_log(SYSTEM, "|   888     888  888ooo88P'    AO8     888  88888[                   |  \n");
            kernel_log(SYSTEM, "|   888     888  888           RN8     888  888`88b.                 |  \n");
            kernel_log(SYSTEM, "|   888    d88'  888           IK8     d88'  888  `88b.              |  \n");
            kernel_log(SYSTEM, "| o@slankdev'   o888o         o888bood8P'   o888o  o888o             |  \n");
            kernel_log(SYSTEM, "|                                                                    |  \n");
            kernel_log(SYSTEM, "|                                    .oooooo..o                      |    \n");
            kernel_log(SYSTEM, "|                                  d8P'    `Y8         @slankdev     |    \n");
            kernel_log(SYSTEM, "|                                  Y88bo.      oooo    ooo  .oooo.o  |    \n");
            kernel_log(SYSTEM, "|                                   `\"Y8888o.   `88.  .8'  d88(  \"8  |  \n");
            kernel_log(SYSTEM, "| @slankdev:                            `\"Y88b   `88..8'   `\"Y88b.   |  \n");
            kernel_log(SYSTEM, "| please follow me                  oo     .d8P    `888'    o.  )88b |    \n");
            kernel_log(SYSTEM, "| on GitHub Twitter,                8""88888P'      .8'     8""888P'     |\n");
            kernel_log(SYSTEM, "|                                              .o..P'                |    \n");
            kernel_log(SYSTEM, "|                                              `Y8P'                 |    \n");
            kernel_log(SYSTEM, "+--------------------------------------------------------------------+\n");
            break;
        default: throw slankdev::exception("not found");
    }
}


} /* ssnlib */
