
#pragma once


#include <string>
#include <slankdev/filefd.h>



namespace slankdev {




/*
 * SAMPLE CODE
 *
 *     using slankdev::filelogger;
 *     std::string filelogger::path = "log.out";
 *     filelogger::get_instance().write("test\n");
 *     filelogger::get_instance().write("%s is pro\n", "palloc");
 *
 */



/*
 * Logger Singlethon class
 */
class filelogger {
    filefd fd;
    filelogger()
    {
        fd.fopen(path.c_str(), "w");
    }
    ~filelogger() {}

public:
    static std::string path;
    filelogger(const filelogger&) = delete;
    filelogger& operator=(const filelogger&) = delete;
    filelogger(filelogger&&) = delete;
    filelogger& operator=(filelogger&&) = delete;

    template <class... ARGS>
    void write(const char* fmt, ARGS... args) { fd.fprintf(fmt, args...); }
    static filelogger& get_instance() {
        static filelogger inst;
        return inst;
    }
};



} /* namespace slankdev */



