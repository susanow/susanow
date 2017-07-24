
#pragma once

#include <string>
#include <vector>
#include <stdio.h>

#include <slankdev/util.h>
#include <slankdev/exception.h>
#include <slankdev/extra/dpdk.h>

#include <ssn_common.h>
#include <ssn_sys.h>
#include <ssn_timer.h>
#include <ssn_native_thread.h>
#include <ssn_green_thread.h>
#include <ssn_port_stat.h>
#include <ssn_log.h>
#include <ssn_ring.h>
#include <ssn_port.h>


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

static void _func_spawner(void* arg)
{
  func* f = reinterpret_cast<func*>(arg);
  f->poll_exe();
}

class func_wk : public func {
  bool run;
 public:
  ssn_ring* prewk_[2];
  ssn_ring* poswk_[2];
  virtual void poll_exe() override
  {
    ssn_log(SSN_LOG_INFO, "func_wk: INCLUDE DELAY\r\n");
    size_t nb_ports = ssn_dev_count();
    rte_mbuf* mbufs[32];
    while (run) {
      for (size_t p=0; p<nb_ports; p++) {
        size_t deqlen = prewk_[p]->deq_bulk((void**)mbufs, 32);
        for (size_t i=0; i<deqlen; i++) {
          // for (size_t j=0; j<100; j++) ; // DELAY
          for (size_t j=0; j<30; j++) ; // DELAY
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
 public:
  ssn_ring* prewk_[2];
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
 public:
  ssn_ring* poswk_[2];
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
 protected:
  size_t mux_;
 public:
  const std::string name;
  std::vector<func*> funcs;
  stage(const char* n) : name(n), mux_(0) {}
  virtual size_t throughput_pps() const = 0;
  virtual ~stage() {}
  virtual func* allocate() = 0;
  virtual void inc()
  {
    func* f = allocate();
    funcs.push_back(f);
    ssize_t lcore_id = get_free_lcore_id();
    if (lcore_id < 0) throw slankdev::exception("no lcore");
    ssn_native_thread_launch(_func_spawner, f, lcore_id);
    mux_ ++ ;
  }
  virtual void dec()
  {
    size_t idx = funcs.size()-1;
    funcs[idx]->stop();
    funcs.erase(funcs.begin() + idx);
    mux_ -- ;
  }
  virtual size_t mux() const { return mux_; }
};

class stage_rx : public stage {
 public:
  ssn_ring* prewk_[2];
  stage_rx(const char* n) : stage(n) {}
  virtual func* allocate() override
  {
    func_rx* rx = new func_rx;
    rx->prewk_[0] = prewk_[0];
    rx->prewk_[1] = prewk_[1];
    return rx;
  }
  virtual size_t throughput_pps() const override
  {
    size_t sum_rx_pps = 0;
    size_t nb_ports = ssn_dev_count();
    for (size_t i=0; i<nb_ports; i++) {
      auto pps = ssn_port_stat_get_cur_rx_pps(i);
      sum_rx_pps += pps;
    }
    return sum_rx_pps;
  }
};

class stage_wk : public stage {
 public:
  stage_wk(const char* n) : stage(n) {}
  ssn_ring* prewk_[2];
  ssn_ring* poswk_[2];
  virtual func* allocate() override
  {
    func_wk* wk = new func_wk;
    wk->prewk_[0] = prewk_[0];
    wk->prewk_[1] = prewk_[1];
    wk->poswk_[0] = poswk_[0];
    wk->poswk_[1] = poswk_[1];
    return wk;
  }
  virtual size_t throughput_pps() const override
  {
    size_t sum_pps = 0;
    size_t nb_ports = ssn_dev_count();
    for (size_t i=0; i<nb_ports; i++) {
      sum_pps += prewk_[i]->opps;
    }
    return sum_pps;
  }
};

class stage_tx : public stage {
 public:
  stage_tx(const char* n) : stage(n) {}
  ssn_ring* poswk_[2];
  virtual func* allocate() override
  {
    func_tx* tx = new func_tx;
    tx->poswk_[0] = poswk_[0];
    tx->poswk_[1] = poswk_[1];
    return tx;
  }
  virtual size_t throughput_pps() const override
  {
    size_t sum_pps = 0;
    size_t nb_ports = ssn_dev_count();
    for (size_t i=0; i<nb_ports; i++) {
      sum_pps += poswk_[i]->opps;
    }
    return sum_pps;
  }
};

class vnf_l2fwd {
 public:
  std::vector<stage*> stages;
  vnf_l2fwd(ssn_ring** prewk, ssn_ring** poswk)
  {
    stage_rx* rx = new stage_rx("rx");
    rx->prewk_[0] = prewk[0];
    rx->prewk_[1] = prewk[1];

    stage_wk* wk = new stage_wk("wk");
    wk->prewk_[0] = prewk[0];
    wk->prewk_[1] = prewk[1];
    wk->poswk_[0] = poswk[0];
    wk->poswk_[1] = poswk[1];

    stage_tx* tx = new stage_tx("tx");
    tx->poswk_[0] = poswk[0];
    tx->poswk_[1] = poswk[1];

    stages.push_back(rx);
    stages.push_back(wk);
    stages.push_back(tx);
  }
  void deploy()
  {
    size_t nb_stages = stages.size();
    for (size_t i=0; i<nb_stages; i++) {
      stages[i]->inc();
    }
  }
  void debug_dump(FILE* fp)
  {
    fprintf(fp, "vnf1 \r\n");
    size_t nb_stages = stages.size();

    size_t stage_perf[nb_stages];
    for (size_t i=0; i<nb_stages; i++) {
      stage_perf[i] = stages[i]->throughput_pps();
    }

    for (size_t i=0; i<nb_stages; i++) {
      void* ptr  = stages[i];
      size_t mux = stages[i]->mux();
      double r = 1.0;
      if (i!=0) r = double(stage_perf[i])/double(stage_perf[i-1]);
      fprintf(fp ," pl[%zd]: name=\"%s\" mux=%-2zd TP=%-10zd red=%lf",
          i, stages[i]->name.c_str(),
          stages[i]->mux(),
          stages[i]->throughput_pps(), r);
      if (r < 0.7) {
        fprintf(fp, " <-- BOTTLE NECK");
      }
      fprintf(fp, "\r\n");
    }
  }
};




