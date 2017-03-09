
#include "vty.h"
// #include "user_cmd.h"
#include "user_kf.h"
#include <slankdev/string.h>




struct Cmd_show : public node {
    struct nd_author : public node {
        nd_author() : node("author") {}
        void function(shell* sh) { sh->Printf("nd_author\r\n"); }
    };
    struct nd_version : public node {
        nd_version() : node("version") {}
        void function(shell* sh) { sh->Printf("nd_version\r\n"); }
    };
    Cmd_show() : node("show")
    {
        commands.push_back(new nd_author);
        commands.push_back(new nd_version);
    }
    void function(shell* sh) { sh->Printf("show\r\n"); }
};

struct Cmd_quit : public node {
    struct nd_sys : public node {
        nd_sys() : node("system") {}
        void function(shell* sh) { sh->Printf("quit system\r\n"); exit(0); }
    };
    struct nd_shell : public node {
        nd_shell() : node("shell") {}
        void function(shell* sh) { sh->Printf("quit system\r\n"); sh->close(); }
    };
    Cmd_quit() : node("quit")
    {
        commands.push_back(new nd_sys);
        commands.push_back(new nd_shell);
    }
    void function(shell* sh) { sh->Printf("quit\r\n"); }
};

struct Cmd_shot : public node {
    Cmd_shot() : node("shot")
    {
    }
    void function(shell* sh) { sh->Printf("shot\r\n"); }
};

struct Cmd_test : public node {
    struct nd_slankdev : public node {
        nd_slankdev() : node("slankdev") {}
        void function(shell* sh) { sh->Printf("slankdev\r\n"); }
    };
    Cmd_test() : node("test")
    { commands.push_back(new nd_slankdev); }
    void function(shell* sh) { sh->Printf("test\r\n"); }
};




int main()
{

    vty::add_keyfunction(new KF_question('?'                ));
    vty::add_keyfunction(new KF_question('\t'               ));
    vty::add_keyfunction(new KF_return  ('\r'               ));
    vty::add_keyfunction(new KF_delete  (0x7f               ));
    vty::add_keyfunction(new KF_ctrl_B  (slankdev::AC_Ctrl_B));

    vty::add_keyfunction(new KF_hist_search_deep   (slankdev::AC_Ctrl_P));
    vty::add_keyfunction(new KF_hist_search_shallow(slankdev::AC_Ctrl_N));

    // vty::add_command(new Cmd_halt("halt"));
    // vty::add_command(new Cmd_quit("quit"));
    vty::add_command(new Cmd_show);
    vty::add_command(new Cmd_shot);
    vty::add_command(new Cmd_quit);
    vty::add_command(new Cmd_test);
    // vty::add_command(new Cmd_show_thread_info("show thread-info"));
    vty::set_port(9999);

    vty vty;
    vty.dispatch();
}


