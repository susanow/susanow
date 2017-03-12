
#include <slankdev/vty.h>

using slankdev::vty;


struct Cmd_show : public slankdev::vty::cmd_node {
    struct author : public cmd_node {
        author() : cmd_node("author") {}
        void function(vty::shell* sh)
        {
            sh->Printf("Hiroki SHIROKURA.\r\n");
            sh->Printf(" Twitter : @slankdev\r\n");
            sh->Printf(" Github  : slankdev\r\n");
            sh->Printf(" Facebook: hiroki.shirokura\r\n");
            sh->Printf(" E-mail  : slank.dev@gmail.com\r\n");
        }
    };
    struct version : public cmd_node {
        version() : cmd_node("version") {}
        void function(vty::shell* sh)
        {
            sh->Printf("Susanow 0.0.0\r\n");
            sh->Printf("Copyright 2017-2020 Hiroki SHIROKURA.\r\n");
        }
    };
    struct thread_info : public cmd_node {
        thread_info() : cmd_node("thread-info") {}
        void function(vty::shell* sh)
        {
            sh->Printf("show thread-info\r\n");
        }
    };
    Cmd_show() : cmd_node("show")
    {
        commands.push_back(new author);
        commands.push_back(new version);
        commands.push_back(new thread_info);
    }
    void function(vty::shell* sh) { sh->Printf("show\r\n"); }
};
struct Cmd_quit : public slankdev::vty::cmd_node {
    Cmd_quit() : cmd_node("quit") {}
    void function(vty::shell* sh) { sh->close(); }
};
struct halt : public slankdev::vty::cmd_node {
    halt() : cmd_node("halt") {}
    void function(vty::shell* sh)
    {
        sh->root_vty->halt();
    }
};


char str[] = "\r\n"
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


int main()
{
    slankdev::vty vty0(9999, str);

    vty0.add_command(new Cmd_show);
    vty0.add_command(new Cmd_quit);
    vty0.add_command(new halt);
    vty0.dispatch();
}
