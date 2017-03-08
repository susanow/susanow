
#pragma once


class Cmd_quit : public Command {
public:
    Cmd_quit(const char* str) : Command(str) {}
    void exec(shell* sh)
    {
        close(sh->fd);
        sh->fd = -1;
        sh->closed = true;
    }
};
class Cmd_halt : public Command {
public:
       Cmd_halt(const char* str) : Command(str) {}
       void exec(shell*)
       {
           exit(0);
       }
};
class Cmd_show : public Command {
public:
    Cmd_show(const char* str) : Command(str) {}
    void exec(shell* sh)
    {
        FILE* fp = fdopen(sh->fd, "w");
        fprintf(fp, "show command\r\n");
        fflush(fp);
    }
};
class Cmd_show_thread_info : public Command {
public:
    Cmd_show_thread_info(const char* str) : Command(str) {}
    void exec(shell* sh)
    {
        FILE* fp = fdopen(sh->fd, "w");
        fprintf(fp, "show thread-info\r\n");
        fflush(fp);
    }
};
