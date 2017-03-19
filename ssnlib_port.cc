


#include <unistd.h>

#include <ssnlib_mempool.h>
#include <ssnlib_ring.h>
#include <ssnlib_log.h>
#include <ssnlib_port_impl.h>
#include <ssnlib_port.h>


namespace ssnlib {



Port::Port(size_t port_id) :
    nb_rx_rings (1),
    nb_tx_rings (1),
    rx_ring_size(128),
    tx_ring_size(512),
    addr     (port_id),
    conf     (port_id),
    stats    (port_id),
    link     (port_id),
    info     (port_id),
    id       (port_id),
    name     ("port" + std::to_string(id))
{
    if (id >= rte_eth_dev_count())
        throw slankdev::exception("invalid port id");
    kernel_log("Construct %s\n", name.c_str());
}


Port::~Port()
{
    fini();
    kernel_log("Destruct %s\n", name.c_str());
}


void Port::init()
{
    kernel_log("Initialize %s\n", name.c_str());
    configure();
    devstart();
    promisc_enable();
}


void Port::fini()
{
    kernel_log("Finilize %s\n", name.c_str());
    devstop();
}


void Port::linkup()
{
    int ret = rte_eth_dev_set_link_up  (id);
    if (ret < 0)
        throw slankdev::exception("rte_eth_dev_link_up: failed");
}




void Port::devstart()
{
    int ret = rte_eth_dev_start(id);
    if (ret < 0)
        throw slankdev::exception("rte_eth_dev_start: failed");
}


void Port::configure()
{

    conf.raw.rxmode.mq_mode = ETH_MQ_RX_RSS;
    conf.raw.rx_adv_conf.rss_conf.rss_key = nullptr;
    conf.raw.rx_adv_conf.rss_conf.rss_hf  = ETH_RSS_IP;

    int retval = rte_eth_dev_configure(id, nb_rx_rings, nb_tx_rings, &conf.raw);
    if (retval != 0)
        throw slankdev::exception("rte_eth_dev_configure failed");

    rxq.reserve(nb_tx_rings);
    for (uint16_t qid=0; qid<nb_rx_rings; qid++) {
        rxq.emplace_back(id, qid, rx_ring_size);
    }
    txq.reserve(nb_tx_rings);
    for (uint16_t qid=0; qid<nb_tx_rings; qid++) {
        txq.emplace_back(id, qid, tx_ring_size);
    }
    rte_eth_macaddr_get(id, &addr);
    info.get();

    kernel_log("Configuration %s %s \n", name.c_str(), addr.toString().c_str());
    kernel_log("  nb_rx_rings=%zd size=%zd\n", nb_rx_rings, rx_ring_size);
    kernel_log("  nb_tx_rings=%zd size=%zd\n", nb_tx_rings, tx_ring_size);
}




} /* namespace ssnlib */




