
#pragma once
#include <ssn_ring.h>
#include <ssn_vty.h>
#include <ssn_rest.h>
#include <ssn_timer.h>

class vnic {
 public:
  const std::string name;
  vnic(const char* n) : name(n) {}
  ssn_ring* rx;
  ssn_ring* tx;
};


class nfvi final {
 public:
  static constexpr uint16_t vty_port = 9999;
  static constexpr uint16_t rest_port = 8888;
  static constexpr size_t   required_nb_cores = 4;
  static constexpr size_t   green_thread_lcoreid = 1;
  static constexpr size_t   timer_lcoreid = 2;
  static constexpr size_t   vswitch_thread_lcoreid = 3;

  ssn_timer_sched* timer_sched;
  ssn_vty* vty;
  ssn_rest* rest;

  std::vector<ssn_ring*> ring_rx;
  std::vector<ssn_ring*> ring_tx;

  nfvi(int argc, char** argv);
  virtual ~nfvi();

  void connect_vv(vnic& nic0, vnic& nic1);
  void connect_vp(vnic& nic0, size_t nic1);
  void green_thread_launch(ssn_function_t f, void* arg);
};


