
#pragma once


class KF_question : public KeyFunc {
public:
    KF_question(char c) : KeyFunc(c) {}
    void function(shell* sh)
    {
        printf("buf=[%s]\n", sh->inputstr.c_str());
        FILE* fp = fdopen(sh->fd, "w");
        fprintf(fp, "\r\n");
        for (Command* cmd : shell::commands) {
            fprintf(fp, "  %s\r\n", cmd->name.c_str());
        }
        fprintf(fp, "%s%s", sh->prompt, sh->inputstr.c_str());
        fflush(fp);
    }
};
class KF_return : public KeyFunc {
public:
    KF_return(char c) : KeyFunc(c) {}
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
    KF_delete(char c) : KeyFunc(c) {}
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
    KF_ctrl_B(char c) : KeyFunc(c) {}
    void function(shell* sh) { char c=0x08; sh->write(&c, 1); }
};
