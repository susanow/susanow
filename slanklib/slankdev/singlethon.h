

#pragma once



namespace slankdev {



/*
 * Template Singlethon Class Implementation
 * - This is code temp.
 *   User should copy this codes and re-write
 */
class CLASSNAME {
    CLASSNAME() {}
    ~CLASSNAME() {}

public:
    CLASSNAME(const filelogger&) = delete;
    CLASSNAME& operator=(const filelogger&) = delete;
    CLASSNAME(filelogger&&) = delete;
    CLASSNAME& operator=(filelogger&&) = delete;

    static CLASSNAME& get_instance() {
        static CLASSNAME inst;
        return inst;
    }
};





} /* namespace slankdev */



