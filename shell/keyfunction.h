
#pragma once


class KeyFunc {
public:
    uint8_t code[256];
    size_t  len;
    KeyFunc(const void* d, size_t l) : len(l)
    {
        if (sizeof(code) < l)
            throw slankdev::exception("FFAAAAA");
        memcpy(code, d, l);
    }
    virtual ~KeyFunc() {}
    virtual void function(shell*) = 0;
    virtual bool match(const void* d, size_t l)
    {
        return (len == l) && (memcmp(code, d, l) == 0);
    }
};


