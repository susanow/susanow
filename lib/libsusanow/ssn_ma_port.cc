
/*
 * MIT License
 *
 * Copyright (c) 2017 Hiroki SHIROKURA
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
/**
 * @details
 *   ssn_ma_port module.
 *   Provide abstruction layer to access physical
 *         dpdk port from multi-threads.
 */

#include <stdio.h>

#include <string>
#include <vector>
#include <utility>

#include <ssn_ma_port.h>
#include <ssn_port.h>
#include <ssn_log.h>
#include <ssn_cpu.h>
#include <ssn_common.h>
#include <ssn_thread.h>
#include <ssn_port_stat.h>

#include <slankdev/exception.h>
#include <slankdev/string.h>
#include <slankdev/exception.h>


/*****************************************************************************\
 * Class Definition are Below...
\*****************************************************************************/

class ssn_ma_port {
 private:

  class ssn_ma_port_oneside {
   private:
    class accessor {
      size_t accessor_idx;
      std::vector<size_t> ques;
     public:
      accessor() {}
      void set(std::vector<size_t>& vec);
      size_t get();
      size_t get_current() const { return ques[accessor_idx]; }
    };
    class ssn_ma_port_queue {
     public:
      size_t que_id;
      size_t acc_id;
    };
   protected:
    size_t dpdk_port_id;
    size_t n_queues_;
    size_t n_accessor;
    std::vector<ssn_ma_port_queue> queues;
    std::vector<accessor> accessors;
   protected:
    std::vector<size_t> get_qids_from_aid(size_t aid) const;
   public:
    ssn_ma_port_oneside() : n_queues_(1), n_accessor(1), accessors(1) {}
    virtual size_t burst(size_t aid, rte_mbuf** mbufs, size_t n_mbufs) = 0;
    size_t get_next_qid_from_aid(size_t aid) const { return accessors[aid].get_current(); }
    size_t get_n_accessor() const { return n_accessor; }
    size_t n_queues() const { return n_queues_; }
    size_t n_queues_per_accessor() const;
    void show() const;
    void configure_queue_accessor(size_t dpdk_pid, size_t n_que, size_t n_acc);
  };
  class ssn_ma_port_oneside_rx : public ssn_ma_port_oneside {
   public:
    ssn_ma_port_oneside_rx() : ssn_ma_port_oneside() {}
    virtual size_t burst(size_t aid, rte_mbuf** mbufs, size_t n_mbufs) override
    {
      size_t qid = accessors[aid].get();
      ssn_log(SSN_LOG_DEBUG,
          "ssn_ma_port: rx_burst(pid=%zd, qid=%zd, access_id=%zd)",
          dpdk_port_id, qid, aid);
      return rte_eth_rx_burst(dpdk_port_id, qid, mbufs, n_mbufs);
    }
  };
  class ssn_ma_port_oneside_tx : public ssn_ma_port_oneside {
   public:
    ssn_ma_port_oneside_tx() : ssn_ma_port_oneside() {}
    virtual size_t burst(size_t aid, rte_mbuf** mbufs, size_t n_mbufs) override
    {
      size_t qid = accessors[aid].get();
      ssn_log(SSN_LOG_DEBUG,
          "ssn_ma_port: tx_burst(pid=%zd, qid=%zd, access_id=%zd)",
          dpdk_port_id, qid, aid);
      return rte_eth_tx_burst(dpdk_port_id, qid, mbufs, n_mbufs);
    }
  };

 private:
  size_t dpdk_pid;
  size_t n_rxq;
  size_t n_txq;
  ssn_ma_port_oneside_rx rx;
  ssn_ma_port_oneside_tx tx;

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

 public:
  size_t n_rx_queue() const { return n_rxq; }
  size_t n_tx_queue() const { return n_txq; }
  size_t n_rx_accessor() const { return rx.get_n_accessor(); }
  size_t n_tx_accessor() const { return tx.get_n_accessor(); }
  size_t get_next_rxqid_from_aid(size_t rxaid) const { return rx.get_next_qid_from_aid(rxaid); }
  size_t get_next_txqid_from_aid(size_t txaid) const { return tx.get_next_qid_from_aid(txaid); }
}; /* class ssn_ma_port */



/*****************************************************************************\
 * Instance Function are Below...
\*****************************************************************************/

void ssn_ma_port::ssn_ma_port_oneside::accessor::set(std::vector<size_t>& vec)
{
  ques = vec;
  accessor_idx = 0;
}

size_t ssn_ma_port::ssn_ma_port_oneside::accessor::get()
{
  size_t ret = accessor_idx;
  accessor_idx = (accessor_idx+1) % ques.size();
  return ques[ret];
}

std::vector<size_t> ssn_ma_port::ssn_ma_port_oneside::get_qids_from_aid(size_t aid) const
{
  std::vector<size_t> vec;
  size_t n_que = queues.size();
  for (size_t i=0; i<n_que; i++) {
    if (queues[i].acc_id == aid) {
      vec.push_back(queues[i].que_id);
    }
  }
  return vec;
}

size_t ssn_ma_port::ssn_ma_port_oneside::n_queues_per_accessor() const
{
  if ((n_queues() % n_accessor) != 0) {
    std::string err = "ssn_ma_port::ssn_ma_port_oneside::n_queues_per_accessor: ";
    err += slankdev::format("n_que=%zd, n_acc=%zd", n_queues(), n_accessor);
    throw slankdev::exception(err.c_str());
  }
  size_t ret = n_queues() / n_accessor;
  return ret;
}

void ssn_ma_port::ssn_ma_port_oneside::show() const
{
  printf("\n");

  printf(" n_queues   : %zd ", n_queues());
  printf(" queues={");
  for (size_t i=0; i<n_queues(); i++)
    printf("%zd%s", i, (i+1<n_queues())?",":"");
  printf("}\n");

  printf(" n_accessor : %zd ", n_accessor);
  printf(" access_ids={");
  for (size_t i=0; i<n_accessor; i++)
    printf("%zd%s", i, (i+1<n_accessor)?",":"");
  printf("}\n");

  printf(" n_queues_per_accessor: %zd \n", n_queues_per_accessor());

  printf("\n");
  printf("  %-4s %-10s \n", "qid", "access_id");
  printf(" ---------------------------------------\n");
  size_t n_que = queues.size();
  for (size_t i=0; i<n_que; i++) {
    printf("  %-4zd %-10zd \n", queues[i].que_id, queues[i].acc_id);
  }
  printf("\n");

  size_t n_acc = n_accessor;
  for (size_t aid=0; aid<n_acc; aid++) {
    auto vec = get_qids_from_aid(aid);

    printf(" aid(%zd).rxqids={", aid);
    auto n_elements = vec.size();
    for (size_t i=0; i<n_elements; i++) {
      printf("%zd%s", vec[i], (i+1<n_elements)?",":"");
    }
    printf("}\n");
  }
  printf("\n");
}

void ssn_ma_port::ssn_ma_port_oneside::configure_queue_accessor(size_t dpdk_pid, size_t n_que, size_t n_acc)
{
  dpdk_port_id = dpdk_pid;
  n_queues_   = n_que;
  n_accessor = n_acc;
  queues.clear();
  accessors.resize(n_acc);

  size_t qid = 0;
  for (size_t aid=0; aid<n_accessor; aid++) {
    const size_t nb_ques_per_access = n_queues_per_accessor();
    for (size_t qid_piv=0; qid_piv<nb_ques_per_access; qid_piv++) {
      queues.push_back({qid+qid_piv, aid});
    }
    qid += nb_ques_per_access;
  }

  for (size_t aid=0; aid<n_accessor; aid++) {
    auto vec = get_qids_from_aid(aid);
    accessors[aid].set(vec);
  }
}

void ssn_ma_port::configure_hwqueue(size_t dpdk_pid_, size_t n_rxq_i, size_t n_txq_i)
{
  dpdk_pid = dpdk_pid_;
  n_rxq = n_rxq_i;
  n_txq = n_txq_i;

  ssn_port_conf conf;
  conf.nb_rxq = n_rxq;
  conf.nb_txq = n_txq;
  conf.raw.rxmode.mq_mode = ETH_MQ_RX_RSS;
  conf.raw.rx_adv_conf.rss_conf.rss_key = NULL;
  conf.raw.rx_adv_conf.rss_conf.rss_hf = ETH_RSS_IP|ETH_RSS_TCP|ETH_RSS_UDP;
  /* conf.debug_dump(stdout); */

  ssn_port_configure(dpdk_pid, &conf);
}

void ssn_ma_port::configure_accessor(size_t n_rxacc, size_t n_txacc)
{
  rx.configure_queue_accessor(dpdk_pid, n_rxq, n_rxacc);
  tx.configure_queue_accessor(dpdk_pid, n_txq, n_txacc);
}

size_t ssn_ma_port::rx_burst(size_t aid, rte_mbuf** mbufs, size_t nb_mbufs)
{ return rx.burst(aid, mbufs, nb_mbufs); }

size_t ssn_ma_port::tx_burst(size_t aid, rte_mbuf** mbufs, size_t nb_mbufs)
{ return tx.burst(aid, mbufs, nb_mbufs); }



/*****************************************************************************\
 * SSN_APIs are Below...
\*****************************************************************************/

/* ssn_ma_port Instances */
ssn_ma_port vnf_ports[RTE_MAX_ETHPORTS];

void ssn_ma_port_link_up(size_t pid)     { ssn_port_link_up    (pid); }
void ssn_ma_port_link_down(size_t pid)   { ssn_port_link_down  (pid); }
void ssn_ma_port_promisc_on(size_t pid)  { ssn_port_promisc_on (pid); }
void ssn_ma_port_promisc_off(size_t pid) { ssn_port_promisc_off(pid); }
void ssn_ma_port_dev_up(size_t pid)      { ssn_port_dev_up     (pid); }
void ssn_ma_port_dev_down(size_t pid)    { ssn_port_dev_down   (pid); }

void ssn_ma_port_configure_hw(size_t port_id, size_t n_rxq, size_t n_txq)
{
  if (port_id >= ssn_dev_count()) {
    std::string err = "ssn_ma_port_configure_hw: ";
    err += slankdev::format("port_id is invalid pid=%zd", port_id);
    throw slankdev::exception(err.c_str());
  }
  vnf_ports[port_id].configure_hwqueue(port_id, n_rxq, n_txq);
}

void ssn_ma_port_configure_acc(size_t port_id, size_t n_rxacc, size_t n_txacc)
{
  if (port_id >= ssn_dev_count()) {
    std::string err = "ssn_ma_port_configure_acc: ";
    err += slankdev::format("port_id is invalid pid=%zd", port_id);
    throw slankdev::exception(err.c_str());
  }
  vnf_ports[port_id].configure_accessor(n_rxacc, n_txacc);
}

size_t ssn_ma_port_rx_burst(size_t port_id, size_t aid, rte_mbuf** mbufs, size_t n_mbufs)
{
  return vnf_ports[port_id].rx_burst(aid,  mbufs, n_mbufs);
}

size_t ssn_ma_port_tx_burst(size_t port_id, size_t aid, rte_mbuf** mbufs, size_t n_mbufs)
{
  return vnf_ports[port_id].tx_burst(aid,  mbufs, n_mbufs);
}

void ssn_ma_port_debug_dump(FILE* fp)
{
  fprintf(fp, " ssn_ma_port_debug_dump \r\n");
}

size_t ssn_ma_port_get_num_rx_hw(size_t port_id) { return vnf_ports[port_id].n_rx_queue(); }
size_t ssn_ma_port_get_num_tx_hw(size_t port_id) { return vnf_ports[port_id].n_tx_queue(); }
size_t ssn_ma_port_get_num_rx_acc(size_t port_id) { return vnf_ports[port_id].n_rx_accessor(); }
size_t ssn_ma_port_get_num_tx_acc(size_t port_id) { return vnf_ports[port_id].n_tx_accessor(); }

size_t ssn_ma_port_get_next_rxqid_from_aid(size_t port_id, size_t aid)
{ return vnf_ports[port_id].get_next_rxqid_from_aid(aid); }

size_t ssn_ma_port_get_next_txqid_from_aid(size_t port_id, size_t aid)
{ return vnf_ports[port_id].get_next_txqid_from_aid(aid); }

