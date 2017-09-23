
#include <ssn_vnf_port.h>
#include <slankdev/string.h>
#include <slankdev/exception.h>


void ssn_vnf_port::ssn_vnf_port_oneside::accessor::set(std::vector<size_t>& vec)
{
  ques = vec;
  accessor_idx = 0;
}

size_t ssn_vnf_port::ssn_vnf_port_oneside::accessor::get()
{
  size_t ret = accessor_idx;
  accessor_idx = (accessor_idx+1) % ques.size();
  return ques[ret];
}

std::vector<size_t> ssn_vnf_port::ssn_vnf_port_oneside::get_qids_from_aid(size_t aid) const
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

size_t ssn_vnf_port::ssn_vnf_port_oneside::n_queues_per_accessor() const
{
  if ((n_queues() % n_accessor) != 0) {
    std::string err = slankdev::format("OKASHII n_que=%zd, n_acc=%zd", n_queues(), n_accessor);
    throw slankdev::exception(err.c_str());
  }
  size_t ret = n_queues() / n_accessor;
  return ret;
}

void ssn_vnf_port::ssn_vnf_port_oneside::show() const
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

void ssn_vnf_port::ssn_vnf_port_oneside::configure_queue_accessor(size_t dpdk_pid, size_t n_que, size_t n_acc)
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

void ssn_vnf_port::configure_hwqueue(size_t dpdk_pid_, size_t n_rxq_i, size_t n_txq_i)
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

void ssn_vnf_port::configure_accessor(size_t n_rxacc, size_t n_txacc)
{
  rx.configure_queue_accessor(dpdk_pid, n_rxq, n_rxacc);
  tx.configure_queue_accessor(dpdk_pid, n_txq, n_txacc);
}

size_t ssn_vnf_port::rx_burst(size_t aid, rte_mbuf** mbufs, size_t nb_mbufs)
{ return rx.burst(aid, mbufs, nb_mbufs); }

size_t ssn_vnf_port::tx_burst(size_t aid, rte_mbuf** mbufs, size_t nb_mbufs)
{ return tx.burst(aid, mbufs, nb_mbufs); }



