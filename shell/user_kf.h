
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
        std::vector<std::string> list = slankdev::split(sh->inputstr, ' ');
        list.push_back("");
        function_impl(sh, list);
        sh->refresh_prompt();
    }
    void append_space(shell* sh)
    {
        if (*(sh->inputstr.end()-1) != ' ') sh->inputstr += " ";
    }
    void remove_space(shell* sh)
    {
        if (*(sh->inputstr.end()-1) == ' ') sh->inputstr.resize(sh->inputstr.size()-1);
    }
    void function_impl(shell* sh, std::vector<std::string>& list)
    {
        sh->Printf("\r\n");
        std::vector<node*>* tree = &sh->commands;
        for (size_t index=0; index < list.size(); index++) {
            /*-----------------------------------------------------------------------------*/

            std::vector<node*> match_nd;
            for (node* nd : *tree) {
                if (strncmp(list[index].c_str(), nd->name.c_str(), list[index].length()) == 0) {
                    match_nd.push_back(nd);
                }
            }

            switch (match_nd.size()) {
                case 0:
                {
                    sh->Printf("  <none>\r\n");
                    return;
                    break;
                }
                case 1:
                {
                    remove_space(sh);

                    size_t d = match_nd[0]->name.length() - list[index].length();
                    std::string cmpstr = match_nd[0]->name.substr(list[index].length(), d);
                    if (list[index].size() == 0)
                        append_space(sh);

                    sh->inputstr += cmpstr;
                    append_space(sh);

                    if (cmpstr.size() > 0) {
                        append_space(sh);
                        return ;
                    }
                    else
                        tree = &match_nd[0]->commands;

                    break;
                }
                default:
                {
                    for (size_t j=sh->inputstr.length(); j<match_nd[0]->name.length(); j++) {
                        char c = match_nd[0]->name[j];
                        for (size_t i=1; i<match_nd.size(); i++) {
                            if (match_nd[i]->name[j] != c) {
                                for (node* nn : match_nd) {
                                    sh->Printf("  %s\r\n", nn->name.c_str());
                                }
                                return ;
                            }
                        }
                        sh->inputstr += c;
                    }
                    break;
                }
            }
            append_space(sh);
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
