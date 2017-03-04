
#include "vty.h"

class Cmd_quit : public Command {
public:
    Cmd_quit(const char* str) : Command(str) {}
    void exec(shell* sh)
    {
        close(sh->fd);
        sh->fd = -1;
        sh->closed = true;
    }
};
class Cmd_halt : public Command {
public:
       Cmd_halt(const char* str) : Command(str) {}
       void exec(shell*)
       {
           exit(0);
       }
};
class KF_question : public KeyFunc {
public:
    KF_question() : KeyFunc('?') {}
    void function(shell* sh) { printf("HATENA buf=\"%s\"\n", sh->inputstr.c_str()); }
};
class KF_return : public KeyFunc {
public:
    KF_return() : KeyFunc('\r') {}
    void function(shell* sh)
    {
        char cs[] = "\r\n";
        sh->write(cs, sizeof(cs));
        sh->exec_command();
        sh->refresh_promptline();
    }
};
class KF_delete : public KeyFunc {
public:
    KF_delete() : KeyFunc(0x7f) {}
    void function(shell* sh)
    {
        char str[] = { 0x08, ' '};
        sh->write(str, sizeof(str));
        if (!sh->inputstr.empty()) {
            sh->inputstr.resize(sh->inputstr.length()-1);
            sh->refresh_promptline();
        }
    }
};
class KF_ctrl_B : public KeyFunc {
public:
    KF_ctrl_B() : KeyFunc(slankdev::AC_Ctrl_B) {}
    void function(shell* sh) { char c=0x08; sh->write(&c, 1); }
};

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


