
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


#if 1
class KF_question : public KeyFunc {
public:
    KF_question(char c) : KeyFunc(c) {}
    void function(shell* sh)
    {
        function_impl(sh);
        sh->refresh_prompt();
    }
    void function_impl(shell* sh)
    {
        printf("buf=[%s]\n", sh->inputstr.c_str());
        sh->Printf("\r\n");

        std::vector<std::string> list = slankdev::split(sh->inputstr, ' ');
        if (list.empty()) {
            for (size_t i=0; i<sh->commands.size(); i++) {
                sh->Printf("  %s\r\n", sh->commands[i]->name.c_str());
            }
            return ;
        }

        size_t index = 0;
        for (std::string& nodestr : list) {
            std::vector<node*> match_nd;
            if (sh->inputstr[sh->inputstr.length()-1] == ' ')
                index ++;
            if (index == 0) {
                /*----------------------------------------*/
                for (node* nd : sh->commands) {
                    if (strncmp(nodestr.c_str(), nd->name.c_str(), nodestr.length()) == 0) {
                        match_nd.push_back(nd);
                    }
                }

                if (match_nd.empty()) {
                    return;
                } else if (match_nd.size() == 1) {
                    printf("compare(%s,%s)\n", match_nd[0]->name.c_str(), nodestr.c_str());
                    if (match_nd[0]->name == nodestr) {
                        if (sh->inputstr[sh->inputstr.length()-1] != ' ') {
                            sh->inputstr += " ";
                            continue;
                        }
                    }
                }

                for (size_t j=sh->inputstr.length(); j<match_nd[0]->name.length(); j++) {
                    char c = match_nd[0]->name[j];
                    for (size_t i=1; i<match_nd.size(); i++) {
                        if (match_nd[i]->name[j] == c) {
                            continue;
                        } else {
                            /*  print  */
                            for (node* nn : match_nd) {
                                sh->Printf("  %s \r\n", nn->name.c_str());
                            }
                            return ;
                        }
                    }
                    sh->inputstr += c;
                }
                /*----------------------------------------*/
            } else {
                /*----------------------------------------*/
                printf("222222222222222222222222\n");
                index ++;
                /*----------------------------------------*/
            }
        }
    }
};
#endif

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
