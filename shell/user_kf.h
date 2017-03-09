
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
    bool debugmode;
public:
    KF_question(char c) : KeyFunc(c), debugmode(true) {}
    void function(shell* sh)
    {
        std::vector<std::string> list = slankdev::split(sh->inputstr, ' ');
        list.push_back("");
        function_impl(sh, list);
        update(sh, list);
        sh->refresh_prompt();
    }
    void append_space(std::string& str)
    {
        if (*(str.end()-1) != ' ') str += " ";
        else dprintf("append IGNORE\n");
    }
    void remove_space(std::string& str)
    {
        if (*(str.end()-1) == ' ') str.resize(str.size()-1);
        else dprintf("remove IGNORE\n");
    }
    void update(shell* sh, std::vector<std::string>& list)
    {
            dprintf("update \n");
            sh->inputstr.clear();
            for (size_t i=0; i<list.size(); i++) {
                sh->inputstr += list[i];
            }
    }
    template <class... ARGS>
    void dprintf(const char* fmt, ARGS... args)
    { if (debugmode) printf(fmt, args...); }

    void function_impl(shell* sh, std::vector<std::string>& list)
    {
        dprintf("===============================================\n");

        sh->Printf("\r\n");
        std::vector<node*>* tree = &sh->commands;
        for (size_t index=0; index < list.size(); index++) {
            /*-----------------------------------------------------------------------------*/
            dprintf("-------------------------\n");

            dprintf("[+] list[%zd]=\"%s\"\n", index, list[index].c_str());

            /*
             * Create Match List
             */
            dprintf("[+] create match list\n");
            std::vector<node*> match_nd;
            for (node* nd : *tree) {
                dprintf(" \"%s\" ", nd->name.c_str());
                if (strncmp(list[index].c_str(), nd->name.c_str(), list[index].length()) == 0) {
                    match_nd.push_back(nd);
                    dprintf("add");
                } else {
                    dprintf("ignore");
                }
                dprintf("\n");
            }
            dprintf("[+] create math list... done\n");

            /*
             * If avalable, Completation
             */
            switch (match_nd.size()) {
                case 0:
                {
                    dprintf("[+] Can't complete \n");
                    sh->Printf("  <none>\r\n");
                    return; // TODO fix
                    break;
                }
                case 1:
                {
                    dprintf("[+] Found 1 complete Item\n");

                    size_t d = match_nd[0]->name.length() - list[index].length();
                    std::string cmpstr = match_nd[0]->name.substr(list[index].length(), d);

                    list[index]  += cmpstr;
                    append_space(list[index]);
                    tree = &match_nd[0]->commands;
                    break;
                }
                default:
                {
                    dprintf("[+] Found many complete Items\n");
                    for (size_t j=list[index].length(); j<match_nd[0]->name.length(); j++) {
                        char c = match_nd[0]->name[j];
                        for (size_t i=1; i<match_nd.size(); i++) {
                            if (match_nd[i]->name[j] != c) {
                                for (node* nn : match_nd) {
                                    sh->Printf("  %s\r\n", nn->name.c_str());
                                }
                                return ;
                            }
                        }
                        list[index]  += c;
                    }
                    break;
                }
            }
            /*-----------------------------------------------------------------------------*/
        }
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
        sh->refresh_prompt();
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
            sh->refresh_prompt();
        }
    }
};
class KF_ctrl_B : public KeyFunc {
public:
    KF_ctrl_B(char c) : KeyFunc(c) {}
    void function(shell* sh) { char c=0x08; sh->write(&c, 1); }
};
