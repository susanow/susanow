
#pragma once


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
