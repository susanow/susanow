
#pragma once


class KF_hist_search_deep : public KeyFunc {
public:
    KF_hist_search_deep(char c) : KeyFunc(c) {}
    void function(shell* sh)
    {
        if (sh->hist_index+1 > sh->history.size()) return;

        FILE* fp = fdopen(sh->fd, "w");
        fprintf(fp, "\r%s", sh->prompt);
        for (size_t i=0; i<sh->inputstr.length(); i++)
            fprintf(fp, " ");
        sh->hist_index++;
        sh->inputstr = sh->history.at(sh->history.size() - sh->hist_index);
        fprintf(fp, "\r%s%s", sh->prompt, sh->inputstr.c_str());
        fflush(fp);
    }
};
class KF_hist_search_shallow : public KeyFunc {
public:
    KF_hist_search_shallow(char c) : KeyFunc(c) {}
    void function(shell* sh)
    {
        if (sh->hist_index = 0) return;

        FILE* fp = fdopen(sh->fd, "w");
        sh->hist_index--;
        sh->inputstr = sh->history.at(sh->history.size() - sh->hist_index);
        fprintf(fp, "\r%s%s", sh->prompt, sh->inputstr.c_str());
        fflush(fp);
    }
};


class KF_question : public KeyFunc {
public:
    KF_question(char c) : KeyFunc(c) {}
    void function(shell* sh)
    {
        printf("buf=[%s]\n", sh->inputstr.c_str());
        FILE* fp = fdopen(sh->fd, "w");
        fprintf(fp, "\r\n");
        const char* str = sh->inputstr.c_str();
        const size_t slen = sh->inputstr.length();

        std::vector<Command*> match_cmds;
        for (Command* cmd : shell::commands) {
            if (slen > cmd->name.length()) continue;
            if (strncmp(str, cmd->name.c_str(), slen) == 0)
                match_cmds.push_back(cmd);
        }
        if (match_cmds.empty()) {
            printf("no match command\n");
            fprintf(fp, "\r%s%s", sh->prompt, sh->inputstr.c_str());
            fflush(fp);
            return ;
        } else if (match_cmds.size() > 1) {
            for (size_t i=slen; i<match_cmds[0]->name.length(); i++) {
                bool flag = true;
                for (Command* cmd : match_cmds) {
                    if (match_cmds[0]->name[i] == cmd->name[i]) {
                        continue;
                    } else {
                        flag = false;
                        break;
                    }
                }
                if (flag) {
                    sh->inputstr += match_cmds[0]->name[i];
                } else {
                    break;
                }
            }

            for (Command* cmd : match_cmds) {
                fprintf(fp, "  %s\r\n", cmd->name.c_str());
            }
        } else {
            sh->inputstr = match_cmds[0]->name;
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
