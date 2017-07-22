


/*
 * @brief Craft format string like a python's string.format(...)
 * @param[out]      fmt   string format
 * @param[in]       args  arguments
 * @retval          std::string crafted format string as a std::string
 */
inline std::string format(const char* fmt, ARGS... args);

/**
 * A structure used to retrieve statistics for an Ethernet port.
 * Not all statistics fields in struct rte_eth_stats are supported
 * by any type of network interface card (NIC). If any statistics
 * field is not supported, its value is 0.
 */
struct rte_eth_stats {
  uint64_t ipackets;  /**< Total number of successfully received packets. */
  uint64_t opackets;  /**< Total number of successfully transmitted packets.*/
  uint64_t ibytes;    /**< Total number of successfully received bytes. */
  uint64_t obytes;    /**< Total number of successfully transmitted bytes. */
  uint64_t imissed;
  /**< Total of RX packets dropped by the HW,
   * because there are no available buffer (i.e. RX queues are full).
   */
  uint64_t ierrors;   /**< Total number of erroneous received packets. */
  uint64_t oerrors;   /**< Total number of failed transmitted packets. */
  uint64_t rx_nombuf; /**< Total number of RX mbuf allocation failures. */
  uint64_t q_ipackets[RTE_ETHDEV_QUEUE_STAT_CNTRS];
  /**< Total number of queue RX packets. */
  uint64_t q_opackets[RTE_ETHDEV_QUEUE_STAT_CNTRS];
  /**< Total number of queue TX packets. */
  uint64_t q_ibytes[RTE_ETHDEV_QUEUE_STAT_CNTRS];
  /**< Total number of successfully received queue bytes. */
  uint64_t q_obytes[RTE_ETHDEV_QUEUE_STAT_CNTRS];
  /**< Total number of successfully transmitted queue bytes. */
  uint64_t q_errors[RTE_ETHDEV_QUEUE_STAT_CNTRS];
  /**< Total number of queue packets received that are dropped. */
};



/**
 * A set of values to identify what method is to be used to transmit
 * packets using multi-TCs.
 */
enum rte_eth_tx_mq_mode {
  ETH_MQ_TX_NONE    = 0,  /**< It is in neither DCB nor VT mode. */
  ETH_MQ_TX_DCB,          /**< For TX side,only DCB is on. */
  ETH_MQ_TX_VMDQ_DCB, /**< For TX side,both DCB and VT is on. */
  ETH_MQ_TX_VMDQ_ONLY,    /**< Only VT on, no DCB */
};



