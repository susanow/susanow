
#pragma once

#include <vector>
#include <slankdev/string.h>

class node {
public:
    const std::string name;
    std::vector<node*> commands;

    node(const char* s) : name(s) {}
    virtual ~node()
    {
        for (node* n : commands)
            delete n;
    }
    virtual void function(shell*) = 0;
    void append_childnode(node* n) { commands.push_back(n); }
    node* next(const char* str)
    {
        for (node* nd : commands) {
            if (nd->name == str) return nd;
        }
        return nullptr;
    }
    node* match(const std::string& str)
    {
        std::vector<std::string> list = slankdev::split(str, ' ');
        node* nd = this;

        if (nd->name != list[0]) return nullptr;
        for (size_t i=1; i<list.size(); i++) {
            nd = nd->next(list[i].c_str());
            if (nd == nullptr) {
                return nullptr;
            }
        }
        return nd;
    }
};



#if 1
// class Command {
// public:
//     node* n;
//     Command(node* nd) : n(nd) {}
//     const char* name() { return n->name.c_str(); }
// };
#else
class Command {
public:
    const std::string name;
    Command(const char* n) : name(n) {}
    virtual ~Command() {}
    virtual void exec(shell*) = 0;
};
#endif


