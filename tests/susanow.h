
#pragma once
#include <slankdev/exception.h>
#include <slankdev/util.h>

#include <ssn_sys.h>
#include <ssn_common.h>
#include <ssn_timer.h>
#include <ssn_native_thread.h>
#include <ssn_green_thread.h>
#include <ssn_vty.h>
#include <ssn_rest.h>

#include "ssn_vnf.h"



void restcb_root(int fd, const void* buf, size_t len, void* a)
{
  slankdev::fdprintf(fd,
      "{"
         "\"avalable_route\" : ["
            "{ \"route\"   : \"author: get statistics data\" },"
            "{ \"route\"   : \"stats : get author infos\"    } "
         "]"
      "}"
      );
}
void restcb_author(int fd, const void* buf, size_t len, void* a)
{
  slankdev::fdprintf(fd,
      "{"
        "\"name\"    : \"Hiroki SHIROKURA\"     ,"
        "\"emal\"    : \"slank.dev@gmail.com\"  ,"
        "\"twitter\" : \"@slankdev\"            ,"
        "\"github\"  : \"slankdev\"              "
      "}"
      );
}
void restcb_stats(int fd, const void* buf, size_t len, void* a)
{
  slankdev::fdprintf(fd,
      "{"
        "\"nb_vnfs\" : 3,"
        "\"nb_nics\" : 4,"
        "\"nb_thds\" : 5 "
      "}"
      );
}
vty_cmd_match vtymt_slank()
{
  vty_cmd_match m;
  m.nodes.push_back(new node_fixedstring("slank", ""));
  return m;
}
void vtycb_slank(vty_cmd_match* m, vty_client* sh, void*)
{
  static size_t cnt = 0;
  sh->Printf("slankdev called %zd times\r\n", cnt++);
}
vty_cmd_match vtymt_show_vnf()
{
  vty_cmd_match m;
  m.nodes.push_back(new node_fixedstring("show", ""));
  m.nodes.push_back(new node_fixedstring("vnf", ""));
  return m;
}
void vtycb_show_vnf(vty_cmd_match* m, vty_client* sh, void* arg)
{
  std::vector<vnf*>& vnfs = *reinterpret_cast<std::vector<vnf*>*>(arg);
  auto nb_vnfs = vnfs.size();
  sh->Printf("VNF show nb=%zd\r\n", nb_vnfs);
  sh->Printf("------\r\n");
  for (auto i=0; i<nb_vnfs; i++) {
    sh->Printf("vnfs[%zd] \"%s\" %p \r\n", i, vnfs[i]->name.c_str(), vnfs[i]);
    FILE* fp = fdopen(sh->get_fd(), "w");
    vnfs[i]->debug_dump(fp);
    fflush(fp);
    sh->Printf("------\r\n");
  }
}
vty_cmd_match vtymt_vnfctl()
{
  vty_cmd_match m;
  m.nodes.push_back(new node_fixedstring("vnfctl", ""));
  return m;
}
vty_cmd_match vtymt_vnfctl_STR_inc()
{
  vty_cmd_match m;
  m.nodes.push_back(new node_fixedstring("vnfctl", ""));
  m.nodes.push_back(new node_string                   );
  m.nodes.push_back(new node_fixedstring("inc", "increment thread muximize"));
  return m;
}
void vtycb_vnfctl_STR_inc(vty_cmd_match* m, vty_client* sh, void* arg)
{
  std::vector<vnf*>& vnfs = *reinterpret_cast<std::vector<vnf*>*>(arg);
  auto nb_vnfs = vnfs.size();
  std::string s = m->nodes[1]->get();
  for (auto i=0; i<nb_vnfs; i++) {
    if (s == vnfs[i]->name) {
      vnfs[i]->pl[0].inc();
      return ;
    }
  }
}

/*
 *---------------------------------------------------
 */

class ssn {
  const uint16_t vty_port = 9999;
  const uint16_t rest_port = 8888;
  const size_t required_nb_cores = 4;
  ssn_timer_sched* timer_sched;
  ssn_vty* vty;
  ssn_rest* rest;
  std::vector<vnf*> vnfs;
 public:
  ssn(int argc, char** argv)
  {
    ssn_init(argc, argv);
    if (ssn_lcore_count() <= required_nb_cores)
      throw slankdev::exception("cpu tarinai");
    ssn_green_thread_sched_register(1);
    timer_sched = new ssn_timer_sched(2);

    vty = new ssn_vty(0x0, vty_port);
    vty->install_command(vtymt_slank(), vtycb_slank, nullptr);
    vty->install_command(vtymt_show_vnf(), vtycb_show_vnf, &vnfs);
    vty->install_command(vtymt_vnfctl(), vtycb_show_vnf, &vnfs);
    vty->install_command(vtymt_vnfctl_STR_inc(), vtycb_vnfctl_STR_inc, &vnfs);
    ssn_green_thread_launch(ssn_vty_poll_thread, vty, 1);

    rest = new ssn_rest(0x0, rest_port);
    rest->add_route("/"       , restcb_root  , nullptr);
    rest->add_route("/stats"  , restcb_stats , nullptr);
    rest->add_route("/author" , restcb_author, nullptr);
    ssn_green_thread_launch(ssn_rest_poll_thread, rest,1);
  }
  virtual ~ssn()
  {
    ssn_green_thread_sched_unregister(1);
    delete timer_sched;
    ssn_wait_all_lcore();
    ssn_fin();
  }
  void vnf_deploy(vnf* nf)
  {
    auto& pl = nf->pl;
    auto nb = nf->pl.size();
    vnfs.push_back(nf);

    for (auto i=0; i<nb; i++) {
      auto lcore_id = get_free_lcore_id();
      if (lcore_id < 0) throw slankdev::exception("no wait core");
      pl[i].inc();
    }
  }
 private:
};

