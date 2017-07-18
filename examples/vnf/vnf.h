
#pragma once

#include <stdio.h>
#include <string>
#include <vector>
#include <ssn_common.h>
#include <ssn_log.h>


static inline ssize_t get_free_lcore_id()
{
  size_t nb_lcores = ssn_lcore_count();
  for (size_t i=1; i<nb_lcores; i++) {
    auto s = ssn_get_lcore_state(i);
    if (s == SSN_LS_WAIT) return i;
  }
  return -1;
}

class func {
 public:
  virtual void poll_exe() = 0;
  virtual void stop() = 0;
};

class func_wk : public func {
  bool run;
  ssn_ring* prewk_[2];
  ssn_ring* poswk_[2];
 public:
  virtual void poll_exe() override
  {
    size_t nb_ports = ssn_dev_count();
    rte_mbuf* mbufs[32];
    while (run) {
      for (size_t p=0; p<nb_ports; p++) {
        size_t deqlen = prewk_[p]->deq_bulk((void**)mbufs, 32);
        for (size_t i=0; i<deqlen; i++) {
          int ret = poswk_[p^1]->enq(mbufs[i]);
          if (ret < 0) rte_pktmbuf_free(mbufs[i]);
        }
      }
    }
  }
  virtual void stop() override { run = false; }
};

class func_rx : public func {
  bool run;
  ssn_ring* prewk_[2];
 public:
  virtual void poll_exe() override
  {
    size_t nb_ports = ssn_dev_count();
    while (run) {
      for (size_t p=0; p<nb_ports; p++) {
        rte_mbuf* mbufs[32];
        size_t recvlen = ssn_port_rx_burst(p, 0, mbufs, 32);
        if (recvlen == 0) continue;
        size_t enqlen = prewk_[p]->enq_bulk((void**)mbufs, recvlen);
        if (recvlen > enqlen) {
          slankdev::rte_pktmbuf_free_bulk(&mbufs[enqlen], recvlen-enqlen);
        }
      }
    }
  }
  virtual void stop() override { run = false; }
};

class func_tx : public func {
  bool run;
  ssn_ring* poswk_[2];
 public:
  virtual void poll_exe() override
  {
    size_t nb_ports = ssn_dev_count();
    while (run) {
      for (size_t p=0; p<nb_ports; p++) {
        rte_mbuf* mbufs[32];
        size_t deqlen = poswk_[p]->deq_bulk((void**)mbufs, 32);
        if (deqlen == 0) continue;
        size_t sendlen = ssn_port_tx_burst(p, 0, mbufs, deqlen);
        if (deqlen > sendlen) {
          slankdev::rte_pktmbuf_free_bulk(&mbufs[sendlen], deqlen-sendlen);
        }
      }
    }
  }
  virtual void stop() override { run = false; }
};

class stage {
 public:
  std::vector<func*> funcs;
  virtual func* allocate() = 0;
  virtual void inc()
  {
    func* f = allocate();
    funcs.push_back(f);
    f->poll_exe();
  }
  virtual void dec()
  {
    size_t idx = funcs.size()-1;
    funcs[idx]->stop();
    funcs.erase(funcs.begin() + idx);
  }
};

class stage_rx : public stage {
 public:
  virtual func* allocate() override { return new func_rx; }
};
class stage_wk : public stage {
 public:
  virtual func* allocate() override { return new func_wk; }
};
class stage_tx : public stage {
 public:
  virtual func* allocate() override { return new func_tx; }
};

class vnf_l2fwd {
  std::vector<stage*> stages;
 public:
  vnf_l2fwd()
  {
    stages.push_back(new stage_rx);
    stages.push_back(new stage_wk);
    stages.push_back(new stage_tx);
  }
};

class pl_stage {
 public:
  ssn_function_t f;
  void* arg;
  size_t mux;
  pl_stage(ssn_function_t ff, void* a) : f(ff), arg(a), mux(0) {}
  size_t perf() const { return 1; }
  void inc()
  {
    ssize_t lcore_id = get_free_lcore_id();
    if (lcore_id < 0) {
      ssn_log(SSN_LOG_INFO, "could'nt increment therad\n");
      return ;
    }
    ssn_native_thread_launch(f, arg, lcore_id);
    ssn_log(SSN_LOG_INFO, "increment therad\n");
    mux++;
  }
};

class vnf {
 public:
  std::vector<pl_stage> pl;
  std::string name;
  vnf(const char* n) : name(n) {}
  virtual ~vnf() {}
  void deploy()
  {
    vnf* nf = this;
    auto& pl = nf->pl;
    auto nb = nf->pl.size();

    for (auto i=0; i<nb; i++) {
      auto lcore_id = get_free_lcore_id();
      if (lcore_id < 0) throw slankdev::exception("no wait core");
      pl[i].inc();
    }
  }
  void debug_dump(FILE* fp)
  {
    fprintf(fp, "\"%s\" %p \r\n", name.c_str(), this);
    size_t nb_pl = pl.size();
    for (size_t i=0; i<nb_pl; i++) {
      fprintf(fp ," pl[%zd]: f=%p, arg=%p mux=%zd perf=%zd \r\n",
          i, pl[i].f, pl[i].arg, pl[i].mux, pl[i].perf());
    }
  }
};


