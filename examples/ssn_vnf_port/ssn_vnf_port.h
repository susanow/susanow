
#pragma once
#include <stdio.h>
#include <stddef.h>
#include <stdint.h>
#include <vector>
#include <utility>
#include <slankdev/exception.h>

#include <ssn_port.h>
#include <ssn_log.h>
#include <ssn_cpu.h>
#include <ssn_common.h>
#include <ssn_thread.h>
#include <ssn_port_stat.h>


class ssn_vnf_port {
 private:

  class ssn_vnf_port_oneside {
   private:
    class accessor {
      size_t accessor_idx;
      std::vector<size_t> ques;
     public:
      accessor() {}
      void set(std::vector<size_t>& vec);
      size_t get();
    };
    class ssn_vnf_port_queue {
     public:
      size_t que_id;
      size_t acc_id;
    };
   protected:
    size_t dpdk_port_id;
    size_t n_queues_;
    size_t n_accessor;
    std::vector<ssn_vnf_port_queue> queues;
    std::vector<accessor> accessors;
   protected:
    std::vector<size_t> get_qids_from_aid(size_t aid) const;
    size_t n_queues_per_accessor() const;
    void show() const;
   public:
    ssn_vnf_port_oneside() : n_queues_(1), n_accessor(1), accessors(1) {}
    virtual size_t burst(size_t aid, rte_mbuf** mbufs, size_t n_mbufs) = 0;
    size_t n_queues() const { return n_queues_; }
    void configure_queue_accessor(size_t dpdk_pid, size_t n_que, size_t n_acc);
  };
  class ssn_vnf_port_oneside_rx : public ssn_vnf_port_oneside {
   public:
    ssn_vnf_port_oneside_rx() : ssn_vnf_port_oneside() {}
    virtual size_t burst(size_t aid, rte_mbuf** mbufs, size_t n_mbufs) override
    {
      size_t qid = accessors[aid].get();
      ssn_log(SSN_LOG_DEBUG,
          "ssn_vnf_port: rx_burst(pid=%zd, qid=%zd, access_id=%zd)",
          dpdk_port_id, qid, aid);
      return rte_eth_rx_burst(dpdk_port_id, qid, mbufs, n_mbufs);
    }
  };
  class ssn_vnf_port_oneside_tx : public ssn_vnf_port_oneside {
   public:
    ssn_vnf_port_oneside_tx() : ssn_vnf_port_oneside() {}
    virtual size_t burst(size_t aid, rte_mbuf** mbufs, size_t n_mbufs) override
    {
      size_t qid = accessors[aid].get();
      ssn_log(SSN_LOG_DEBUG,
          "ssn_vnf_port: tx_burst(pid=%zd, qid=%zd, access_id=%zd)",
          dpdk_port_id, qid, aid);
      return rte_eth_tx_burst(dpdk_port_id, qid, mbufs, n_mbufs);
    }
  };

 private:
  size_t dpdk_pid;
  size_t n_rxq;
  size_t n_txq;
  ssn_vnf_port_oneside_rx rx;
  ssn_vnf_port_oneside_tx tx;

 public:
  void configure_hwqueue(size_t dpdk_pid_, size_t n_rxq_i, size_t n_txq_i);
  void configure_accessor(size_t n_rxacc, size_t n_txacc);
  size_t rx_burst(size_t aid, rte_mbuf** mbufs, size_t nb_mbufs);
  size_t tx_burst(size_t aid, rte_mbuf** mbufs, size_t nb_mbufs);
  void link_up()       { ssn_port_link_up    (dpdk_pid); }
  void link_down()     { ssn_port_link_down  (dpdk_pid); }
  void promisc_on()  { ssn_port_promisc_on (dpdk_pid); }
  void promisc_off() { ssn_port_promisc_off(dpdk_pid); }
  void dev_up()      { ssn_port_dev_up     (dpdk_pid); }
  void dev_down()    { ssn_port_dev_down   (dpdk_pid); }
};


