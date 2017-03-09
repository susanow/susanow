
#pragma once



struct Cmd_show : public node {
    struct nd_author : public node {
        nd_author() : node("author") {}
        void function(shell* sh) { sh->Printf("nd_author\r\n"); }
    };
    struct nd_version : public node {
        nd_version() : node("version") {}
        void function(shell* sh) { sh->Printf("nd_version\r\n"); }
    };
    Cmd_show() : node("show")
    {
        commands.push_back(new nd_author);
        commands.push_back(new nd_version);
    }
    void function(shell* sh) { sh->Printf("show\r\n"); }
};

struct Cmd_quit : public node {
    struct nd_sys : public node {
        nd_sys() : node("system") {}
        void function(shell* sh) { sh->Printf("quit system\r\n"); exit(0); }
    };
    struct nd_shell : public node {
        nd_shell() : node("shell") {}
        void function(shell* sh)
        {
            sh->Printf("quit system\r\n");
            sh->closed = true;
        }
    };
    Cmd_quit() : node("quit")
    {
        commands.push_back(new nd_sys);
        commands.push_back(new nd_shell);
    }
    void function(shell* sh) { sh->Printf("quit\r\n"); }
};

struct Cmd_shot : public node {
    Cmd_shot() : node("shot") {}
    void function(shell* sh) { sh->Printf("shot\r\n"); }
};

struct Cmd_test : public node {
    struct nd_slankdev : public node {
        nd_slankdev() : node("slankdev") {}
        void function(shell* sh) { sh->Printf("slankdev\r\n"); }
    };
    Cmd_test() : node("test")
    { commands.push_back(new nd_slankdev); }
    void function(shell* sh) { sh->Printf("test\r\n"); }
};
