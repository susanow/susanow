
#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <poll.h>
#include <fcntl.h>
#include <time.h>
#include <poll.h>
#include <fcntl.h>
#include <netinet/tcp.h>

#include <string>
#include <vector>

#include <slankdev/util.h>
#include <slankdev/string.h>
#include <slankdev/socketfd.h>
#include <slankdev/exception.h>
#include <slankdev/asciicode.h>
#include <slankdev/telnet.h>




namespace slankdev {




/*
 * Sample Code
 *
 *      slankdev::vty vty0(9999, str);
 *      vty0.add_command(new Cmd_show);
 *      vty0.dispatch();
 *
 */

/*
 * class Cmd_show : public slankdev::vty::cmd_node {
 *     struct author : public cmd_node {
 *         author() : cmd_node("author") {}
 *         void function(vty::shell* sh)
 *         {
 *             sh->Printf("Hiroki SHIROKURA.\r\n");
 *             sh->Printf(" Twitter : @slankdev\r\n");
 *             sh->Printf(" Github  : slankdev\r\n");
 *             sh->Printf(" Facebook: hiroki.shirokura\r\n");
 *             sh->Printf(" E-mail  : slank.dev@gmail.com\r\n");
 *         }
 *     };
 *     struct version : public cmd_node {
 *         version() : cmd_node("version") {}
 *         void function(vty::shell* sh)
 *         {
 *             sh->Printf("Susanow 0.0.0\r\n");
 *             sh->Printf("Copyright 2017-2020 Hiroki SHIROKURA.\r\n");
 *         }
 *     };
 * public:
 *     Cmd_show() : cmd_node("show")
 *     {
 *         commands.push_back(new author);
 *         commands.push_back(new version);
 *     }
 *     void function(vty::shell* sh) { sh->Printf("show\r\n"); }
 * };
 */


class vty {
public:
    class shell {
        friend class vty;
        std::string prompt;
        int fd;
        size_t cur_idx;
        bool closed;

        std::string ibuf;
        void press_keys(const void* d, size_t l);
        std::string name();
    public:
        vty* root_vty;
        size_t hist_index;
        std::vector<std::string> history;

        shell(vty* v, int d, const char* bootmsg);
        void close() { closed = true; }
        void exec_command();
        void clean_prompt();
        void refresh_prompt();
        template <class... ARGS> void Printf(const char* fmt, ARGS... args)
        { slankdev::fdprintf(fd, fmt, args...); }

        /*
         * Input-Buffer Control Functions
         */
        void input_char(char c) { ibuf.insert(ibuf.begin()+cur_idx, c); cur_idx++; }
        void input_str(std::string& str) { for (char c : str) input_char(c); }
        void buffer_clear()  { ibuf.clear(); cur_idx = 0; }
        size_t buffer_length()   const { return ibuf.length(); }
        const char* get_buffer() const { return ibuf.c_str();  }

        /*
         * Cursor Control Functions
         */
        void cursor_right() { cur_idx ++ ; }
        void cursor_left() { cur_idx -- ; }
        void cursor_backspace();

        int process();
    };
    class cmd_node {
    public:
        const std::string name;
        std::vector<cmd_node*> commands;
        cmd_node(const char* s) : name(s) {}
        virtual ~cmd_node() { for (cmd_node* n : commands) delete n; }
        virtual void function(shell*) = 0;
        cmd_node* next(const char* str);
        cmd_node* match(const std::string& str);
        void append_childcmd_node(cmd_node* n) { commands.push_back(n); }
    };
    class key_func {
    public:
        uint8_t code[256];
        size_t  len;
        key_func(const void* d, size_t l) : len(l)
        {
            if (sizeof(code) < l)
                throw slankdev::exception("FFAAAAA");
            memcpy(code, d, l);
        }
        virtual ~key_func() {}
        virtual void function(shell*) = 0;
        virtual bool match(const void* d, size_t l)
        {
            return (len == l) && (memcmp(code, d, l) == 0);
        }
    };
private:
    bool running;
    uint16_t port;
    const std::string bootmsg;
    int server_fd;
    std::vector<shell> shells;
    std::vector<cmd_node*> commands;
    std::vector<key_func*> keyfuncs;
public:
    void* user_ptr;

    vty(uint16_t p, const char* bootmsg);
    virtual ~vty()
    {
        for (cmd_node* c : commands) delete c;
        for (key_func* f : keyfuncs) delete f;
    }
    void add_keyfunction(key_func* kf);
    void add_command(cmd_node* cmd);
    void init_default_keyfunc();
    void dispatch();
    void halt() { running = false; }
    std::vector<cmd_node*>& get_cmds() { return commands; }
    void add_default_keyfunctions();
};




/*
 * Default key_functions Implementation
 */
struct KF_return : public vty::key_func {
    KF_return(const void* c, size_t l) : vty::key_func(c, l) {}
    void function(vty::shell* sh) { sh->exec_command(); }
};
struct KF_backspace : public vty::key_func {
    KF_backspace(const void* c, size_t l) : key_func(c, l) {}
    void function(vty::shell* sh) { sh->cursor_backspace(); }
};
struct KF_right : public vty::key_func {
    KF_right(const void* c, size_t l) : key_func(c, l) {}
    void function(vty::shell* sh) { sh->cursor_right(); }
};
struct KF_left : public vty::key_func {
    KF_left(const void* c, size_t l) : key_func(c, l) {}
    void function(vty::shell* sh) { sh->cursor_left(); }
};
struct KF_hist_search_deep : public vty::key_func {
    KF_hist_search_deep(const void* c, size_t l) : key_func(c, l) {}
    void function(vty::shell* sh)
    {
        if (sh->hist_index+1 > sh->history.size()) return;

        sh->buffer_clear();
        sh->input_str(sh->history.at(sh->history.size() - 1 - sh->hist_index));
        sh->hist_index++;
        sh->refresh_prompt();
    }
};
struct KF_hist_search_shallow : public vty::key_func {
    KF_hist_search_shallow(const void* c, size_t l) : key_func(c, l) {}
    void function(vty::shell* sh)
    {
        if (sh->hist_index == 0) return;

        sh->buffer_clear();
        sh->input_str(sh->history.at(sh->history.size() - sh->hist_index));
        sh->hist_index--;
        sh->refresh_prompt();
    }
};
class KF_completion : public vty::key_func {
    bool debugmode;
    void append_space(std::string& str)
    {
        if (str.empty()) return;
        if (*(str.end()-1) != ' ') { str += " "; }
    }
    void remove_space(std::string& str)
    {
        if (*(str.end()-1) == ' ') str.resize(str.size()-1);
    }
    void update(vty::shell* sh, std::vector<std::string>& list)
    {
            dprintf("[+] update \n");
            sh->buffer_clear();
            for (size_t i=0; i<list.size(); i++) {
                sh->input_str(list[i]);
            }
    }
    template <class... ARGS>
    void dprintf(const char* fmt, ARGS... args)
    { if (debugmode) printf(fmt, args...); }
    void function_impl(vty::shell* sh, std::vector<std::string>& list);
public:
    KF_completion(const void* c, size_t l) : key_func(c, l), debugmode(false) {}
    void function(vty::shell* sh)
    {
        std::vector<std::string> list = slankdev::split(sh->get_buffer(), ' ');
        list.push_back("");
        function_impl(sh, list);
        update(sh, list);
        sh->refresh_prompt();
    }
};





/*
 * Function Definition
 */
inline void KF_completion::function_impl(vty::shell* sh, std::vector<std::string>& list)
{
    dprintf("===============================================\n");

    sh->Printf("\r\n");
    std::vector<vty::cmd_node*>* tree = &(sh->root_vty->get_cmds());
    for (size_t index=0; index < list.size(); index++) {
        dprintf("-------------------------\n");
        dprintf("[+] list[%zd]=\"%s\"\n", index, list[index].c_str());

        /*
         * Create Match List
         */
        dprintf("[+] create match list\n");
        std::vector<vty::cmd_node*> match_nd;
        for (vty::cmd_node* nd : *tree) {
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
                            for (vty::cmd_node* nn : match_nd) {
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
    }
}














/*
 * vty::cmd_node's Memeber Function Definition
 */
inline vty::cmd_node* vty::cmd_node::next(const char* str)
{
    for (cmd_node* nd : commands)
    { if (nd->name == str) return nd; }
    return nullptr;
}
inline vty::cmd_node* vty::cmd_node::match(const std::string& str)
{
    std::vector<std::string> list = slankdev::split(str, ' ');
    cmd_node* nd = this;

    if (nd->name != list[0]) return nullptr;
    for (size_t i=1; i<list.size(); i++) {
        nd = nd->next(list[i].c_str());
        if (nd == nullptr) {
            return nullptr;
        }
    }
    return nd;
}






/*
 * vty::shell's Member Functinon Definition
 */
inline vty::shell::shell(vty* v, int d, const char* bootmsg) :
    prompt(name()),
    fd(d),
    cur_idx(0),
    closed(false),
    root_vty(v),
    hist_index(0)
{
    Printf(bootmsg);
    refresh_prompt();
}
inline std::string vty::shell::name()
{
    static int c = 0;
    std::string str = "Susanow" + std::to_string(c++) + "> ";
    return str;
}
inline int vty::shell::process()
{
    char str[100];
    ssize_t res = ::read(fd, str, sizeof(str));
    if (res <= 0) return -1;

    press_keys(str, res);
    refresh_prompt();
    return 1;
}
inline void vty::shell::refresh_prompt()
{
    char lineclear[] = {slankdev::AC_ESC, '[', 2, slankdev::AC_K, '\0'};
    Printf("\r%s", lineclear);
    Printf("\r%s%s", prompt.c_str(), ibuf.c_str());

    size_t backlen = ibuf.length() - cur_idx;
    char left [] = {slankdev::AC_ESC, '[', slankdev::AC_D};
    for (size_t i=0; i<backlen; i++) {
        Printf("%s", left);
    }
}
inline void vty::shell::exec_command()
{
    Printf("\r\n");
    if (!ibuf.empty()) {
        for (cmd_node* c : root_vty->commands) {
            cmd_node* nd = c->match(ibuf);
            if (nd) {
                nd->function(this);
                history.push_back(ibuf);
                goto fin;
            }
        }
        Printf("command not found: \"%s\"\r\n", ibuf.c_str());
    }
fin:
    ibuf.clear();
    hist_index = 0;
    cur_idx = 0;
    Printf("\r%s%s", prompt.c_str(), ibuf.c_str());
    refresh_prompt();
}
inline void vty::shell::press_keys(const void* d, size_t l)
{
    const uint8_t* p = reinterpret_cast<const uint8_t*>(d);
    if (l == 0) throw slankdev::exception("empty data received");

    for (key_func* kf : root_vty->keyfuncs) {
        if (kf->match(p, l)) {
            kf->function(this);
            return ;
        }
    }

    if (l > 1) {
        return ;
    }
    input_char(p[0]);
}
inline void vty::shell::cursor_backspace()
{
    if (cur_idx > 0) {
        cur_idx --;
        ibuf.erase(ibuf.begin() + cur_idx);
    }
}







/*
 * vty's Member Functinon Definition
 */
inline vty::vty(uint16_t p, const char* msg) : running(false), port(p), bootmsg(msg)
{
    slankdev::socketfd server_sock;
    server_sock.noclose_in_destruct = true;
    server_sock.socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    int option = 1;
    server_sock.setsockopt(SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option));
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port   = htons(port);
    addr.sin_addr.s_addr = INADDR_ANY;
    server_sock.bind((sockaddr*)&addr, sizeof(addr));
    server_sock.listen(5);
    server_fd = server_sock.get_fd();

    add_default_keyfunctions();
}
inline void vty::dispatch()
{
    running = true;
    while (running) {
        struct Pollfd : public pollfd {
            Pollfd(int ifd, short ievents)
            {
                fd = ifd;
                events = ievents;
            }
        };
        std::vector<struct Pollfd> fds;
        fds.push_back(Pollfd(server_fd, POLLIN));
        for (shell& sh : shells) fds.emplace_back(Pollfd(sh.fd, POLLIN));

        if (poll(fds.data(), fds.size(), 1000)) {
            if (fds[0].revents & POLLIN) {
                /*
                 * Server Accept Process
                 */
                struct sockaddr_in client;
                socklen_t client_len = sizeof(client);
                int fd = accept(fds[0].fd, (sockaddr*)&client, &client_len);

                slankdev::socketfd client_sock(fd);
                uint32_t on = 1;
                client_sock.setsockopt(IPPROTO_TCP, TCP_NODELAY, &on, sizeof(on));
                int flags = client_sock.fcntl(F_GETFL);
                client_sock.fcntl(F_SETFL, (flags | O_NONBLOCK));
                client_sock.noclose_in_destruct = true;
                slankdev::vty_will_echo (fd);
                slankdev::vty_will_suppress_go_ahead (fd);
                slankdev::vty_dont_linemode (fd);
                slankdev::vty_do_window_size (fd);

                shells.push_back(shell(this, fd, bootmsg.c_str()));
#ifdef DEBUG
                printf("Connected new client. now, nb_shells=%zd\n", shells.size());
#endif
            }

            /*
             * Client Read Process
             */
            for (size_t i=1; i<fds.size(); i++) {
                if (fds[i].revents & POLLIN) {
                    int res = shells[i-1].process();
                    if (shells[i-1].closed || res<=0) {
                        close(fds[i].fd);
                        shells.erase(shells.begin() + i);
#ifdef DEBUG
                        printf("Disconnect client. nb_shells=%zd\n", shells.size());
#endif
                        continue;
                    }
                }
            }
        }
    }
}
inline void vty::add_keyfunction(key_func* kf)
{
    keyfuncs.push_back(kf);
#ifdef DEBUG
    printf("Add Keyfunction ");
    for (size_t i=0; i<kf->len; i++) {
        printf("0x%02x ", kf->code[i]);
    }
    printf("\n");
#endif
}
inline void vty::add_command(vty::cmd_node* cmd)
{
    commands.push_back(cmd);
#ifdef DEBUG
    printf("Add Command, \"%s\"\n", cmd->name.c_str());
#endif
}
inline void vty::add_default_keyfunctions()
{
    using namespace slankdev;

    uint8_t up   [] = {AC_ESC, '[', AC_A};
    uint8_t down [] = {AC_ESC, '[', AC_B};
    uint8_t right[] = {AC_ESC, '[', AC_C};
    uint8_t left [] = {AC_ESC, '[', AC_D};
    add_keyfunction(new KF_hist_search_deep   (up   , sizeof(up   )));
    add_keyfunction(new KF_hist_search_shallow(down , sizeof(down )));
    add_keyfunction(new KF_right(right, sizeof(right)));
    add_keyfunction(new KF_left (left , sizeof(left )));

    uint8_t ctrlP[] = {AC_Ctrl_P};
    uint8_t ctrlN[] = {AC_Ctrl_N};
    uint8_t ctrlF[] = {AC_Ctrl_F};
    uint8_t ctrlB[] = {AC_Ctrl_B};
    add_keyfunction(new KF_hist_search_deep   (ctrlP, sizeof(ctrlP)));
    add_keyfunction(new KF_hist_search_shallow(ctrlN, sizeof(ctrlN)));
    add_keyfunction(new KF_right(ctrlF, sizeof(ctrlF)));
    add_keyfunction(new KF_left (ctrlB, sizeof(ctrlB)));

    uint8_t d1[] = {'?'};
    uint8_t d5[] = {'\t'};
    add_keyfunction(new KF_completion(d1, sizeof(d1)));
    add_keyfunction(new KF_completion(d5, sizeof(d5)));

    uint8_t d2[] = {'\r', '\0'};
    add_keyfunction(new KF_return  (d2, sizeof(d2)));

    uint8_t d3[] = {0x7f};
    add_keyfunction(new KF_backspace  (d3, sizeof(d3)));
}



} /* namepace slankdev */
