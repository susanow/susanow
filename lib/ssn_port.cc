
#include <stdio.h>
#include <susanow.h>
#include <ssn_port.h>
#include <slankdev/extra/dpdk.h>

rte_mempool* mp[RTE_MAX_ETHPORTS];

size_t ssn_dev_count()
{
  return rte_eth_dev_count();
}

void ssn_port_link_up(size_t p)
{
  int ret = rte_eth_dev_set_link_up(p);
  if (ret < 0) {
    ssn_log(SSN_LOG_CRIT, "link up miss\n");
    throw slankdev::exception("ssn_port_link_up");
  }
  ssn_log(SSN_LOG_INFO, "port%zd link up\n", p);
}

void ssn_port_link_down(size_t p)
{
  ssn_log(SSN_LOG_INFO, "port%zd link down\n", p);
  rte_eth_dev_set_link_down(p);
}

void ssn_port_promisc_on(size_t pid)
{
  rte_eth_promiscuous_enable(pid);
}

void ssn_port_promisc_off(size_t pid)
{
  rte_eth_promiscuous_disable(pid);
}

void ssn_port_dev_up(size_t pid)
{
  int ret = rte_eth_dev_start(pid);
  if (ret < 0) {
    throw slankdev::exception("eth dev start");
  }
  ssn_log(SSN_LOG_INFO, "port%zd dev up\n", pid);
}

void ssn_port_dev_down(size_t pid)
{
  rte_eth_dev_stop(pid);
  ssn_log(SSN_LOG_INFO, "port%zd dev down\n", pid);
}

ssn_port_conf::ssn_port_conf()
  : nb_rxq(1), nb_txq(1), nb_rxd(128), nb_txd(512)
{
  slankdev::init_portconf(&raw);
}

void ssn_port_configure(size_t pid, ssn_port_conf* conf)
{
  int ret;
  ssn_log(SSN_LOG_INFO,
      "port%zd configure nb_rxqs=%zd, nb_txqs=%zd, nb_rxd=%zd, nb_txd=%zd\n",
      pid, conf->nb_rxq, conf->nb_txq, conf->nb_rxd, conf->nb_txd);
  ssn_port_dev_down(pid);

  ret = rte_eth_dev_configure(pid, conf->nb_rxq, conf->nb_txq, &conf->raw);
  if (ret < 0) {
    throw slankdev::exception("dev configure");
  }
  for (size_t q=0; q<conf->nb_rxq; q++) {
    ret = rte_eth_rx_queue_setup(pid, q, conf->nb_rxd, rte_eth_dev_socket_id(pid), nullptr, mp[pid]);
    if (ret < 0) {
      throw slankdev::exception("dev rxq setup");
    }
  }
  for (size_t q=0; q<conf->nb_txq; q++) {
    ret = rte_eth_tx_queue_setup(pid, q, conf->nb_txd, rte_eth_dev_socket_id(pid), nullptr);
    if (ret < 0) {
      throw slankdev::exception("dev txq setup");
    }
  }
}

void ssn_port_init()
{
  size_t nb_ports = rte_eth_dev_count();
  for (size_t i=0; i<nb_ports; i++) {
    std::string name = slankdev::format("RXMP%zd", i);
    mp[i] = slankdev::mp_alloc(name.c_str());
  }
}

void ssn_port_fin()
{
  size_t nb_ports = rte_eth_dev_count();
  for (size_t i=0; i<nb_ports; i++) {
    rte_mempool_free(mp[i]);
    ssn_port_link_down(i);
    ssn_port_dev_down(i);
  }
}

void ssn_mbuf_free_bulk(rte_mbuf** m_list, size_t npkts)
{
  while (npkts--)
    rte_pktmbuf_free(*m_list++);
}

size_t ssn_port_rx_burst(size_t pid, size_t qid, rte_mbuf** mbufs, size_t nb_mbufs)
{
  size_t nb_recv = rte_eth_rx_burst(pid, qid, mbufs, nb_mbufs);
  return nb_recv;
}

size_t ssn_port_tx_burst(size_t pid, size_t qid, rte_mbuf** mbufs, size_t nb_mbufs)
{
  size_t nb_send = rte_eth_tx_burst(pid, qid, mbufs, nb_mbufs);
  return nb_send;
}

