
#pragma once


class KeyFunc {
public:
    const char code;
    KeyFunc(char c) : code(c) {}
    virtual ~KeyFunc() {}
    virtual void function(shell*) = 0;
};
