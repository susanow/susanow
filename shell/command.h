
#pragma once

#include <vector>


class node {
public:
    const std::string name;
    std::vector<node*> childs;

    node(const char* s) : name(s) {}
    void append_childnode(node* n) { childs.push_back(n); }
    virtual ~node() {}
    virtual void function(shell*) { printf("not set\n"); };
    node* next(const char* str)
    {
        for (node* nd : childs) {
            if (nd->name == str) return nd;
        }
        return nullptr;
    }
};



#if 1
class Command {
public:
    node* n;
    virtual bool match(const std::string&) = 0;
    virtual void exec(shell* sh, const std::string& str) = 0;
};
#else
class Command {
public:
    const std::string name;
    Command(const char* n) : name(n) {}
    virtual ~Command() {}
    virtual void exec(shell*) = 0;
};
#endif


