
#include "vty.h"
// #include "user_cmd.h"
#include "user_kf.h"
#include <slankdev/string.h>


class nd_author : public node {
public:
    nd_author() : node("author") {}
    void function()
    {
        printf("nd_author\n");
    }
};

class nd_version : public node {
public:
    nd_version() : node("version") {}
    void function()
    {
        printf("nd_version\n");
    }
};


class nd_show : public node {
public:
    nd_show() : node("show")
    {
        append_childnode(new nd_author);
        append_childnode(new nd_version);
    }
};


class Cmd_show : public Command {
public:
    Cmd_show() {
        n = new nd_show;
    }
    bool match(const std::string& str)
    {
        std::vector<std::string> list = slankdev::split(str, ' ');
        printf("\n\n");
        for (size_t i=0; i<list.size(); i++)
            printf("%s \n", list[i].c_str());
        printf("\n\n");
        return true;
    }
    void exec(shell* sh, const std::string& str)
    {
        printf("show\n");
    }
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


