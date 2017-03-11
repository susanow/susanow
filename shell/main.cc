
#include "vty.h"
#include "user_cmd.h"
#include "user_kf.h"
#include <slankdev/string.h>



void keyfunction_init()
{
    using namespace slankdev;

    uint8_t up   [] = {AC_ESC, '[', AC_A};
    uint8_t down [] = {AC_ESC, '[', AC_B};
    uint8_t right[] = {AC_ESC, '[', AC_C};
    uint8_t left [] = {AC_ESC, '[', AC_D};
    vty::add_keyfunction(new KF_hist_search_deep   (up   , sizeof(up   )));
    vty::add_keyfunction(new KF_hist_search_shallow(down , sizeof(down )));
    vty::add_keyfunction(new KF_right(right, sizeof(right)));
    vty::add_keyfunction(new KF_left (left , sizeof(left )));

    uint8_t ctrlP[] = {AC_Ctrl_P};
    uint8_t ctrlN[] = {AC_Ctrl_N};
    uint8_t ctrlF[] = {AC_Ctrl_F};
    uint8_t ctrlB[] = {AC_Ctrl_B};
    vty::add_keyfunction(new KF_hist_search_deep   (ctrlP, sizeof(ctrlP)));
    vty::add_keyfunction(new KF_hist_search_shallow(ctrlN, sizeof(ctrlN)));
    vty::add_keyfunction(new KF_right(ctrlF, sizeof(ctrlF)));
    vty::add_keyfunction(new KF_left (ctrlB, sizeof(ctrlB)));

    uint8_t d1[] = {'?'};
    uint8_t d5[] = {'\t'};
    vty::add_keyfunction(new KF_question(d1, sizeof(d1)));
    vty::add_keyfunction(new KF_question(d5, sizeof(d5)));

    uint8_t d2[] = {'\r', '\0'};
    vty::add_keyfunction(new KF_return  (d2, sizeof(d2)));

    uint8_t d3[] = {0x7f};
    vty::add_keyfunction(new KF_delete  (d3, sizeof(d3)));
}


void command_init()
{
    vty::add_command(new Cmd_show);
    vty::add_command(new Cmd_quit);
}


int main()
{
    keyfunction_init();
    command_init();
    vty::set_port(9999);

    vty vty;
    vty.dispatch();
}



