
#pragma once


class KF_hist_search_deep : public KeyFunc {
public:
    KF_hist_search_deep(const void* c, size_t l) : KeyFunc(c, l) {}
    void function(shell* sh)
    {
        if (sh->hist_index+1 > sh->history.size()) return;

        sh->buffer_clear();
        sh->input_str_to_buffer(sh->history.at(sh->history.size() - 1 - sh->hist_index));
        sh->hist_index++;
        sh->refresh_prompt();
    }
};

class KF_hist_search_shallow : public KeyFunc {
public:
    KF_hist_search_shallow(const void* c, size_t l) : KeyFunc(c, l) {}
    void function(shell* sh)
    {
        if (sh->hist_index == 0) return;

        sh->buffer_clear();
        sh->input_str_to_buffer(sh->history.at(sh->history.size() - sh->hist_index));
        sh->hist_index--;
        sh->refresh_prompt();
    }
};


class KF_question : public KeyFunc {
    bool debugmode;
public:
    KF_question(const void* c, size_t l) : KeyFunc(c, l), debugmode(false) {}
    void function(shell* sh)
    {
        std::vector<std::string> list = slankdev::split(sh->buffer_c_str(), ' ');
        list.push_back("");
        function_impl(sh, list);
        update(sh, list);
        sh->refresh_prompt();
    }
    void append_space(std::string& str)
    {
        if (*(str.end()-1) != ' ') str += " ";
    }
    void remove_space(std::string& str)
    {
        if (*(str.end()-1) == ' ') str.resize(str.size()-1);
    }
    void update(shell* sh, std::vector<std::string>& list)
    {
            dprintf("[+] update \n");
            sh->buffer_clear();
            for (size_t i=0; i<list.size(); i++) {
                sh->input_str_to_buffer(list[i]);
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
                    for (size_t i=0; i<list.size(); i++) {
                        append_space(list[i]);
                    }
                    return;
                    break;
                }
                case 1:
                {
                    dprintf("[+] Found 1 complete Item\n");
                    list[index] = match_nd[0]->name;
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
    KF_return(const void* c, size_t l) : KeyFunc(c, l) {}
    void function(shell* sh)
    {
        sh->Printf("\r\n");
        sh->exec_command();
        sh->refresh_prompt();
    }
};

class KF_delete : public KeyFunc {
public:
    KF_delete(const void* c, size_t l) : KeyFunc(c, l) {}
    void function(shell* sh)
    {
        sh->cursor_backspace();
    }
};

class KF_up : public KeyFunc {
public:
    KF_up(const void* c, size_t l) : KeyFunc(c, l) {}
    void function(shell* sh)
    {
        printf("up\n");
    }
};

class KF_down : public KeyFunc {
public:
    KF_down(const void* c, size_t l) : KeyFunc(c, l) {}
    void function(shell* sh)
    {
        printf("down\n");
    }
};

class KF_right : public KeyFunc {
public:
    KF_right(const void* c, size_t l) : KeyFunc(c, l) {}
    void function(shell* sh)
    {
        sh->cursor_right();
    }
};

class KF_left : public KeyFunc {
public:
    KF_left(const void* c, size_t l) : KeyFunc(c, l) {}
    void function(shell* sh)
    {
        sh->cursor_left();
    }
};

