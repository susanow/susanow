

/*-
 * MIT License
 *
 * Copyright (c) 2017 Susanoo G
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
 * @file ssnlib_misc.h
 * @brief include misc function
 * @author slankdev
 */


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
