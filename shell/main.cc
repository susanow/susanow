
#include "vty.h"
// #include "user_cmd.h"
#include "user_kf.h"
#include <slankdev/string.h>




class Cmd_show : public Command {
    class nd_show : public node {
        class nd_author : public node {
        public:
            nd_author() : node("author") {}
            void function(shell* sh) { sh->Printf("nd_author\r\n"); }
        };
        class nd_version : public node {
        public:
            nd_version() : node("version") {}
            void function(shell* sh) { sh->Printf("nd_version\r\n"); }
        };
    public:
        nd_show() : node("show")
        {
            append_childnode(new nd_author);
            append_childnode(new nd_version);
        }
        void function(shell* sh) { sh->Printf("show\r\n"); }
    };
public:
    Cmd_show() : Command(new nd_show) {}
};



int main()
{

    // vty::add_keyfunction(new KF_question('?'                ));
    // vty::add_keyfunction(new KF_question('\t'               ));
    vty::add_keyfunction(new KF_return  ('\r'               ));
    vty::add_keyfunction(new KF_delete  (0x7f               ));
    vty::add_keyfunction(new KF_ctrl_B  (slankdev::AC_Ctrl_B));

    vty::add_keyfunction(new KF_hist_search_deep   (slankdev::AC_Ctrl_P));
    vty::add_keyfunction(new KF_hist_search_shallow(slankdev::AC_Ctrl_N));

    // vty::add_command(new Cmd_halt("halt"));
    // vty::add_command(new Cmd_quit("quit"));
    vty::add_command(new Cmd_show);
    // vty::add_command(new Cmd_show_thread_info("show thread-info"));
    vty::set_port(9999);

    vty vty;
    vty.dispatch();
}


