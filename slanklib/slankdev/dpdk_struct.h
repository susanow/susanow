

/*-
 * MIT License
 *
 * Copyright (c) 2017 Susanoo G
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
 * @file ssnlib_dpdk.h
 * @brief
 * include debug functions that
 * print many structs defined in dpdk
 * @author slankdev
 */

#pragma once
#include <slankdev/dpdk_header.h>
#include <slankdev/util.h>


namespace slankdev {

#define DEPTHSTR "    "
#define for_i(begin, end) for(size_t i=begin; i<end; i++)



template<class... Args>
inline void printf_depth(size_t depth, const char* const fmt, Args... args)
{
    for_i (0, depth) printf(DEPTHSTR);
    printf(fmt, args...);
}

inline const char* rte_lcore_state_t2str(rte_lcore_state_t e)
{
    switch (e) {
        case WAIT:     return "WAIT"    ;
        case RUNNING : return "RUNNING";
        case FINISHED: return "FINISHED";
        default: return "UNKNOWN_ERROR";
    }
}



inline const char* rte_eth_fc_mode2str(rte_eth_fc_mode e)
{
    switch (e) {
        case RTE_FC_NONE    : return "NONE    ";
        case RTE_FC_RX_PAUSE: return "RX_PAUSE";
        case RTE_FC_TX_PAUSE: return "TX_PAUSE";
        case RTE_FC_FULL    : return "FULL    ";
        default: return "UNKNOWN_ERROR";
    }
}



inline const char* rte_devtype2str(rte_devtype e)
{
    switch (e) {
        case RTE_DEVTYPE_WHITELISTED_PCI: return "WHITELISTED_PCI";
        case RTE_DEVTYPE_BLACKLISTED_PCI: return "BLACKLISTED_PCI";
        case RTE_DEVTYPE_VIRTUAL        : return "VIRTUAL        ";
        default: return "UNKNOWN_ERROR";
    }
}

inline const char* rte_intr_handle_type2str(rte_intr_handle_type e)
{
    switch (e) {
        case RTE_INTR_HANDLE_UNKNOWN    : return "UNKNOWN    ";
        case RTE_INTR_HANDLE_UIO        : return "UIO        ";
        case RTE_INTR_HANDLE_UIO_INTX   : return "UIO_INTX   ";
        case RTE_INTR_HANDLE_VFIO_LEGACY: return "VFIO_LEGACY";
        case RTE_INTR_HANDLE_VFIO_MSI   : return "VFIO_MSI   ";
        case RTE_INTR_HANDLE_VFIO_MSIX  : return "VFIO_MSIX  ";
        case RTE_INTR_HANDLE_ALARM      : return "ALARM      ";
        case RTE_INTR_HANDLE_EXT        : return "EXT        ";
        case RTE_INTR_HANDLE_MAX        : return "MAX        ";
        default: return "UNKNOWN_ERROR";
    }
}

inline const char* rte_eth_nb_tcs2str(rte_eth_nb_tcs e)
{
    switch (e) {
        case ETH_4_TCS: return "4_TCS";
        case ETH_8_TCS: return "8_TCS";
        default: return "UNKNOWN_ERROR";
    }
}

inline const char* rte_kernel_driver2str(rte_kernel_driver e)
{
    switch (e) {
        case RTE_KDRV_UNKNOWN    : return "UNKNOWN    ";
        case RTE_KDRV_IGB_UIO    : return "IGB_UIO    ";
        case RTE_KDRV_VFIO       : return "VFIO       ";
        case RTE_KDRV_UIO_GENERIC: return "UIO_GENERIC";
        case RTE_KDRV_NIC_UIO    : return "NIC_UIO    ";
        case RTE_KDRV_NONE       : return "NONE       ";
        default: return "UNKNOWN_ERROR";
    }
}


inline const char* nb_queue_pools2str(rte_eth_nb_pools e)
{
    switch (e) {
        case ETH_8_POOLS : return "8_POOLS ";
        case ETH_16_POOLS: return "16_POOLS";
        case ETH_32_POOLS: return "32_POOLS";
        case ETH_64_POOLS: return "64_POOLS";
        default: return "UNKNOWN_ERROR";
    }
}


inline const char* rte_eth_payload_type2str(enum rte_eth_payload_type e)
{
    switch (e) {
        case RTE_FDIR_MODE_NONE 	       : return "NONE 	         ";
        case RTE_FDIR_MODE_SIGNATURE 	   : return "SIGNATURE 	     ";
        case RTE_FDIR_MODE_PERFECT 	       : return "PERFECT 	     ";
        case RTE_FDIR_MODE_PERFECT_MAC_VLAN: return "PERFECT_MAC_VLAN";
        case RTE_FDIR_MODE_PERFECT_TUNNEL  : return "PERFECT_TUNNEL  ";
        default: return "UNKNOWN_ERROR";
    }
}




inline const char* rte_eth_tx_mq_mode2str(enum rte_eth_tx_mq_mode mode)
{
    switch (mode) {
        case ETH_MQ_TX_NONE     : return "ETH_MQ_TX_NONE     ";
        case ETH_MQ_TX_DCB      : return "ETH_MQ_TX_DCB      ";
        case ETH_MQ_TX_VMDQ_DCB : return "ETH_MQ_TX_VMDQ_DCB ";
        case ETH_MQ_TX_VMDQ_ONLY: return "ETH_MQ_TX_VMDQ_ONLY";
        default: return "UNKNOWN_ERROR";
    }
}



inline const char* rte_eth_rx_mq_mode2str(enum rte_eth_rx_mq_mode mode)
{
    switch (mode) {
        case ETH_MQ_RX_NONE        : return "NONE ";
        case ETH_MQ_RX_RSS         : return "RSS  ";
        case ETH_MQ_RX_DCB         : return "DCB  ";
        case ETH_MQ_RX_DCB_RSS     : return "DCB_RSS     ";
        case ETH_MQ_RX_VMDQ_ONLY   : return "VMDQ_ONLY   ";
        case ETH_MQ_RX_VMDQ_RSS    : return "VMDQ_RSS    ";
        case ETH_MQ_RX_VMDQ_DCB    : return "VMDQ_DCB    ";
        case ETH_MQ_RX_VMDQ_DCB_RSS: return "VMDQ_DCB_RSS";
        default: return "UNKNOWN_ERROR";
    }
}


inline const char* linkspeed2str(uint32_t speed)
{
	switch (speed) {
        case ETH_LINK_SPEED_AUTONEG:  return "AUTONEG";
        case ETH_LINK_SPEED_FIXED  :  return "FIXED  ";
        case ETH_LINK_SPEED_10M_HD :  return "10M_HD ";
        case ETH_LINK_SPEED_10M    :  return "10M    ";
        case ETH_LINK_SPEED_100M_HD:  return "100M_HD";
        case ETH_LINK_SPEED_100M   :  return "100M   ";
        case ETH_LINK_SPEED_1G     :  return "1G     ";
        case ETH_LINK_SPEED_2_5G   :  return "2_5G   ";
        case ETH_LINK_SPEED_5G     :  return "5G     ";
        case ETH_LINK_SPEED_10G    :  return "10G    ";
        case ETH_LINK_SPEED_20G    :  return "20G    ";
        case ETH_LINK_SPEED_25G    :  return "25G    ";
        case ETH_LINK_SPEED_40G    :  return "40G    ";
        case ETH_LINK_SPEED_50G    :  return "50G    ";
        case ETH_LINK_SPEED_56G    :  return "56G    ";
        case ETH_LINK_SPEED_100G   :  return "100G   ";
        default: return "UNKNOWN_ERROR";
	}
}


inline const char* rte_eth_nb_pools2str(enum rte_eth_nb_pools e)
{
    switch (e) {
		case ETH_8_POOLS : return "ETH_8_POOLS ";
		case ETH_16_POOLS: return "ETH_16_POOLS";
		case ETH_32_POOLS: return "ETH_32_POOLS";
		case ETH_64_POOLS: return "ETH_64_POOLS";
        default: return "UNKNOWN_ERROR";
    }
}


inline const char* rte_fdir_mode2str(enum rte_fdir_mode e)
{
    switch (e) {
        case RTE_FDIR_MODE_NONE            : return "NONE            ";
        case RTE_FDIR_MODE_SIGNATURE       : return "SIGNATURE       ";
        case RTE_FDIR_MODE_PERFECT         : return "PERFECT         ";
        case RTE_FDIR_MODE_PERFECT_MAC_VLAN: return "PERFECT_MAC_VLAN";
        case RTE_FDIR_MODE_PERFECT_TUNNEL  : return "PERFECT_TUNNEL  ";
        default: return "UNKNOWN_ERROR";
    }
}


inline const char* rte_fdir_pballoc_type2str(enum rte_fdir_pballoc_type e)
{
    switch (e) {
        case RTE_FDIR_PBALLOC_64K : return "64K ";
        case RTE_FDIR_PBALLOC_128K: return "128K";
        case RTE_FDIR_PBALLOC_256K: return "256K";
        default: return "UNKNOWN_ERROR";
    }
}


inline const char* rte_fdir_status_mode2str(enum rte_fdir_status_mode e)
{
    switch (e) {
        case RTE_FDIR_NO_REPORT_STATUS    : return "NO_REPORT_STATUS    ";
        case RTE_FDIR_REPORT_STATUS       : return "REPORT_STATUS       ";
        case RTE_FDIR_REPORT_STATUS_ALWAYS: return "REPORT_STATUS_ALWAYS";
        default: return "UNKNOWN_ERROR";
    }
}


inline void print(const struct rte_eth_stats* raw, const char* name="", size_t depth=0)
{
    printf_depth(depth, "rte_eth_stats %s {\n", name);
    printf_depth(depth, "ipackets  : %lu \n", raw->ipackets );
    printf_depth(depth, "opackets  : %lu \n", raw->opackets );
    printf_depth(depth, "ibytes    : %lu \n", raw->ibytes   );
    printf_depth(depth, "obytes    : %lu \n", raw->obytes   );
    printf_depth(depth, "imissed   : %lu \n", raw->imissed  );
    printf_depth(depth, "ierrors   : %lu \n", raw->ierrors  );
    printf_depth(depth, "oerrors   : %lu \n", raw->oerrors  );
    printf_depth(depth, "rx_nombuf : %lu \n", raw->rx_nombuf);

    for_i (0, RTE_ETHDEV_QUEUE_STAT_CNTRS) {
        printf_depth(depth, "q_ipackets[%2zd]   : %lu \n", i, raw->q_ipackets[i]);
        printf_depth(depth, "q_ibytes[%2zd]     : %lu \n", i, raw->q_ibytes  [i]);
    }
    for_i (0, RTE_ETHDEV_QUEUE_STAT_CNTRS) {
        printf_depth(depth, "q_opackets[%2zd]   : %lu \n", i, raw->q_opackets[i]);
        printf_depth(depth, "q_obytes  [%2zd]   : %lu \n", i, raw->q_obytes  [i]);
        printf_depth(depth, "q_errors  [%2zd]   : %lu \n", i, raw->q_errors  [i]);
    }
    printf_depth(depth, "}\n");
}


inline void print(const struct rte_eth_rxmode* raw, const char* name="", size_t depth=0)
{
    printf_depth(depth, "rte_eth_rxmode %s {\n", name);

    depth++;
    printf_depth(depth, "mq_mode        : %s\n", rte_eth_rx_mq_mode2str(raw->mq_mode));
    printf_depth(depth, "max_rx_pkt_len : %u\n", raw->max_rx_pkt_len);
    printf_depth(depth, "split_hdr_size : %u\n", raw->split_hdr_size);
    printf_depth(depth, "header_split   : %s\n", (raw->header_split  ==1)?"Yes":"No");
    printf_depth(depth, "hw_ip_checksum : %s\n", (raw->hw_ip_checksum==1)?"Yes":"No");
    printf_depth(depth, "hw_vlan_filter : %s\n", (raw->hw_vlan_filter==1)?"Yes":"No");
    printf_depth(depth, "hw_vlan_strip  : %s\n", (raw->hw_vlan_strip ==1)?"Yes":"No");
    printf_depth(depth, "hw_vlan_extend : %s\n", (raw->hw_vlan_extend==1)?"Yes":"No");
    printf_depth(depth, "jumbo_frame    : %s\n", (raw->jumbo_frame   ==1)?"Yes":"No");
    printf_depth(depth, "hw_strip_crc   : %s\n", (raw->hw_strip_crc  ==1)?"Yes":"No");
    printf_depth(depth, "enable_scatter : %s\n", (raw->enable_scatter==1)?"Yes":"No");
    printf_depth(depth, "enable_lro     : %s\n", (raw->enable_lro    ==1)?"Yes":"No");
    depth--;

    printf_depth(depth, "}\n");
}

inline void print(const struct rte_eth_txmode* raw, const char* name="", size_t depth=0)
{
    printf_depth(depth, "rte_eth_txmode %s {\n", name);

    depth++;
    printf_depth(depth, "mq_mode                 : %s\n", rte_eth_tx_mq_mode2str(raw->mq_mode)   );
    printf_depth(depth, "hw_vlan_reject_tagged   : %s\n", (raw->hw_vlan_reject_tagged  ==1)?"Yes":"No");
    printf_depth(depth, "hw_vlan_reject_untagged : %s\n", (raw->hw_vlan_reject_untagged==1)?"Yes":"No");
    printf_depth(depth, "hw_vlan_insert_pvid     : %s\n", (raw->hw_vlan_insert_pvid    ==1)?"Yes":"No");
    depth--;

    printf_depth(depth, "}\n");
}


inline void print(const struct rte_eth_rss_conf* raw, const char* name="", size_t depth=0)
{
    printf_depth(depth, "rte_eth_rss_conf %s {\n", name);

    depth++;
    printf_depth(depth, "rss_key     : %p \n", raw->rss_key);
    if (raw->rss_key) {
        printf_depth(depth, "val         : ");
        for_i (0, 40) printf_depth(depth, "%02x", raw->rss_key[i]);
        printf_depth(depth, "\n");
    }
    printf_depth(depth, "rss_key_len : %u\n", raw->rss_key_len   );
    printf_depth(depth, "rss_hf      : %lu\n", raw->rss_hf);
    depth--;

    printf_depth(depth, "}\n");
}




inline void print(const struct rte_eth_vmdq_dcb_conf* raw, const char* name="", size_t depth=0)
{

    printf_depth(depth, "rte_eth_vmdq_dcb_conf %s {\n", name);

	depth++;
	printf_depth(depth, "nb_queue_pools      : %s \n", rte_eth_nb_pools2str(raw->nb_queue_pools));
	printf_depth(depth, "enable_default_pool : %u \n", raw->enable_default_pool);
	printf_depth(depth, "default_pool        : %u \n", raw->default_pool       );
	printf_depth(depth, "nb_pool_maps        : %u \n", raw->nb_pool_maps       );

    for (size_t i=0; i<1; i++) {
        printf_depth(depth, "pool_map[0] {\n", ETH_VMDQ_MAX_VLAN_FILTERS);
        printf_depth(depth, DEPTHSTR "vlan_id : %u \n", raw->pool_map[i].vlan_id);
        printf_depth(depth, DEPTHSTR "pools   : %u \n", raw->pool_map[i].pools  );
        printf_depth(depth, "}\n");
    }
	printf_depth(depth, "pool_map[1-%zd] {} --> EXPANDABLE: see source \n", ETH_VMDQ_MAX_VLAN_FILTERS);

	printf_depth(depth, "dcb_tc : {%u, %u, %u, %u, %u, %u, %u, %u} \n",
            raw->dcb_tc[0], raw->dcb_tc[1], raw->dcb_tc[2], raw->dcb_tc[3],
            raw->dcb_tc[4], raw->dcb_tc[5], raw->dcb_tc[6], raw->dcb_tc[7]);
	depth--;

    printf_depth(depth, "}\n");
}





inline void print(const struct rte_eth_dcb_rx_conf* raw, const char* name="", size_t depth=0)
{
    printf_depth(depth, "rte_eth_dcb_rx_conf %s {\n", name);

	depth++;
	printf_depth(depth, "nb_tcs : %s \n", rte_eth_nb_tcs2str(raw->nb_tcs));
	printf_depth(depth, "dcb_tc : {%u, %u, %u, %u, %u, %u, %u, %u} \n",
            raw->dcb_tc[0], raw->dcb_tc[1], raw->dcb_tc[2], raw->dcb_tc[3],
            raw->dcb_tc[4], raw->dcb_tc[5], raw->dcb_tc[6], raw->dcb_tc[7]);
	depth--;

    printf_depth(depth, "}\n");
}



inline void print(const struct rte_eth_vmdq_rx_conf* raw, const char* name="", size_t depth=0)
{
    printf_depth(depth, "rte_eth_vmdq_rx_conf %s {\n", name);

	depth++;
	printf_depth(depth, "nb_queue_pools : %s \n", rte_eth_nb_pools2str(raw->nb_queue_pools));
	printf_depth(depth, "enable_default_pool : %u \n", raw->enable_default_pool);
	printf_depth(depth, "default_pool        : %u \n", raw->default_pool       );
	printf_depth(depth, "enable_loop_back    : %u \n", raw->enable_loop_back   );
	printf_depth(depth, "nb_pool_maps        : %u \n", raw->nb_pool_maps       );
	printf_depth(depth, "rx_mode             : %u \n", raw->rx_mode            );

	printf_depth(depth, "struct {\n");
	printf_depth(depth, DEPTHSTR "vlan_id : %u \n", raw->pool_map[0].vlan_id);
	printf_depth(depth, DEPTHSTR "pools   : %u \n", raw->pool_map[0].pools  );
	printf_depth(depth, "} pool_map[0]\n");
	printf_depth(depth, "struct {} pool_map[1-%zd]\n", ETH_VMDQ_MAX_VLAN_FILTERS);
	depth--;

    printf_depth(depth, "}\n");
}


inline void print(const struct rte_eth_vmdq_dcb_tx_conf* raw, const char* name="", size_t depth=0)
{
    printf_depth(depth, "rte_eth_vmdq_dcb_tx_conf %s {\n", name);

	depth++;
	printf_depth(depth, "nb_queue_pools : %s \n", rte_eth_nb_pools2str(raw->nb_queue_pools));
	printf_depth(depth, "dcb_tc : {%u, %u, %u, %u, %u, %u, %u, %u} \n",
            raw->dcb_tc[0], raw->dcb_tc[1], raw->dcb_tc[2], raw->dcb_tc[3],
            raw->dcb_tc[4], raw->dcb_tc[5], raw->dcb_tc[6], raw->dcb_tc[7]);
	depth--;

    printf_depth(depth, "}\n");
}


inline void print(const struct rte_eth_dcb_tx_conf* raw, const char* name="", size_t depth=0)
{
    printf_depth(depth, "rte_eth_dcb_tx_conf %s {\n", name);

	depth++;
	printf_depth(depth, "nb_tcs : %s \n", rte_eth_nb_tcs2str(raw->nb_tcs));
	printf_depth(depth, "dcb_tc[] %u \n", raw->dcb_tc[0]);
	printf_depth(depth, "dcb_tc[1-%zd] {} \n", ETH_DCB_NUM_USER_PRIORITIES);
	depth--;

    printf_depth(depth, "}\n");
}

inline void print(const struct rte_eth_vmdq_tx_conf* raw, const char* name="", size_t depth=0)
{
    printf_depth(depth, "rte_eth_vmdq_tx_conf %s {\n", name);

	depth++;
	printf_depth(depth, "nb_queue_pools : %s \n", raw->nb_queue_pools);
	depth--;

    printf_depth(depth, "}\n");
}



inline void print(const struct rte_eth_flex_payload_cfg* raw, const char* name="", size_t depth=0)
{
    printf_depth(depth, "rte_eth_flex_payload_cfg %s {\n", name);

	depth++;
	printf_depth(depth, "type : %s \n", rte_eth_payload_type2str(raw->type));
	printf_depth(depth,
            "src_offset[%zd] : {%u, %u, %u, %u, %u, %u, %u, %u, %u, %u, %u, %u, %u, %u, %u, %u} \n",
            RTE_ETH_FDIR_MAX_FLEXLEN,
            raw->src_offset[0] , raw->src_offset[1] , raw->src_offset[2] , raw->src_offset[3] ,
            raw->src_offset[4] , raw->src_offset[5] , raw->src_offset[6] , raw->src_offset[7] ,
            raw->src_offset[8] , raw->src_offset[9] , raw->src_offset[10], raw->src_offset[11],
            raw->src_offset[12], raw->src_offset[13], raw->src_offset[14], raw->src_offset[15]);
	depth--;

    printf_depth(depth, "}\n");
}
inline void print(const struct rte_eth_fdir_flex_mask* raw, const char* name="", size_t depth=0)
{
    printf_depth(depth, "rte_eth_fdir_flex_mask %s {\n", name);

	depth++;
	printf_depth(depth,
            "mask[%zd] : {%u, %u, %u, %u, %u, %u, %u, %u, %u, %u, %u, %u, %u, %u, %u, %u} \n",
            RTE_ETH_FDIR_MAX_FLEXLEN,
            raw->mask[0] , raw->mask[1] , raw->mask[2] , raw->mask[3] ,
            raw->mask[4] , raw->mask[5] , raw->mask[6] , raw->mask[7] ,
            raw->mask[8] , raw->mask[9] , raw->mask[10], raw->mask[11],
            raw->mask[12], raw->mask[13], raw->mask[14], raw->mask[15]);
	depth--;

    printf_depth(depth, "}\n");
}
inline void print(const struct rte_eth_fdir_flex_conf* raw, const char* name="", size_t depth=0)
{
    printf_depth(depth, "rte_eth_fdir_flex_conf %s {\n", name);

	depth++;
	printf_depth(depth, "nb_payloads  : %u \n", raw->nb_payloads );
	printf_depth(depth, "nb_flexmasks : %u \n", raw->nb_flexmasks);

    print(&raw->flex_set[0], "flex_set[0]", depth);
    printf_depth(depth, "rte_eth_flex_payload_cfg flex_set[1-%zd] {} \n", RTE_ETH_PAYLOAD_MAX);
    print(&raw->flex_mask[0], "flex_mask[0]", depth);
    printf_depth(depth, "rte_eth_fdir_flex_mask flex_mask[1-%zd] {} \n", RTE_ETH_FLOW_MAX);

	depth--;

    printf_depth(depth, "}\n");
}
inline void print(const struct rte_fdir_conf* raw, const char* name="", size_t depth=0)
{
    printf_depth(depth, "rte_fdir_conf %s {\n", name);

	depth++;
	printf_depth(depth, "mode       : %s \n", rte_fdir_mode2str(raw->mode   ));
	printf_depth(depth, "pballoc    : %s \n", rte_fdir_pballoc_type2str(raw->pballoc));
	printf_depth(depth, "status     : %s \n", rte_fdir_status_mode2str(raw->status ));
	printf_depth(depth, "drop_queue : %u \n", raw->drop_queue);
	print(&raw->flex_conf, "flex_conf", depth);
	depth--;

    printf_depth(depth, "}\n");
}



inline void print(const struct rte_intr_conf* raw, const char* name="", size_t depth=0)
{
    printf_depth(depth, "rte_intr_conf %s {\n", name);

	depth++;
	printf_depth(depth, "lsc : %u \n", raw->lsc);
	printf_depth(depth, "rxq : %u \n", raw->rxq);
	depth--;

    printf_depth(depth, "}\n");
}



inline void print(const struct rte_eth_conf* raw, const char* name="", size_t depth=0)
{
    printf_depth(depth, "rte_eth_conf %s {\n", name);

    depth++;
    printf_depth(depth, "link_speeds : %u(%s) \n", raw->link_speeds, linkspeed2str(raw->link_speeds));
    print(&raw->rxmode, "rxmode", depth);
    print(&raw->txmode, "txmode", depth);
    printf_depth(depth, "lpbk_mode   : %u \n", raw->lpbk_mode   );
    printf_depth(depth, "struct {\n");
    print(&raw->rx_adv_conf.rss_conf     , "rss_conf"     , depth+1);
	print(&raw->rx_adv_conf.vmdq_dcb_conf, "vmdq_dcb_conf", depth+1);
    print(&raw->rx_adv_conf.dcb_rx_conf  , "dcb_rx_conf"  , depth+1);
    print(&raw->rx_adv_conf.vmdq_rx_conf , "vmdq_rx_conf" , depth+1);
    printf_depth(depth, "} rx_adv_conf\n");
    printf_depth(depth, "struct {\n");
	printf_depth(depth, DEPTHSTR "[UNION]\n");
    print(&raw->tx_adv_conf.vmdq_dcb_tx_conf, "vmdq_dcb_tx_conf", depth+1);
    print(&raw->tx_adv_conf.dcb_tx_conf     , "dcb_tx_conf"     , depth+1);
    print(&raw->tx_adv_conf.vmdq_tx_conf    , "vmdq_tx_conf"    , depth+1);
    printf_depth(depth, "} tx_adv_conf\n");
    printf_depth(depth, "dcb_capability_en : %u \n", raw->dcb_capability_en  );
    print(&raw->fdir_conf, "fdir_conf", depth);
    print(&raw->intr_conf, "intr_conf", depth);
    depth--;

    printf_depth(depth, "}\n");
}


inline void print(const struct rte_eth_thresh* raw, const char* name="", size_t depth=0)
{
    printf_depth(depth, "rte_eth_thresh %s {\n", name);

    depth++;
    printf_depth(depth, "pthresh : %u  \n", raw->pthresh);
    printf_depth(depth, "hthresh : %u  \n", raw->hthresh);
    printf_depth(depth, "wthresh : %u  \n", raw->wthresh);
    depth--;

    printf_depth(depth, "}\n");
}


inline void print(const struct rte_eth_rxconf* raw, const char* name="", size_t depth=0)
{
    printf_depth(depth, "rte_eth_rxconf %s {\n", name);

    depth++;
    print(&raw->rx_thresh, "rx_thresh", depth);
    printf_depth(depth, "rx_free_thresh    : %u \n", raw->rx_free_thresh   );
    printf_depth(depth, "rx_drop_en        : %u \n", raw->rx_drop_en       );
    printf_depth(depth, "rx_deferred_start : %u \n", raw->rx_deferred_start);
    depth--;

    printf_depth(depth, "}\n");
}


inline void print(const struct rte_eth_txconf* raw, const char* name="", size_t depth=0)
{
    printf_depth(depth, "rte_eth_txconf %s {\n", name);

    depth++;
    print(&raw->tx_thresh, "tx_thresh", depth);
    printf_depth(depth, "tx_rs_thresh      : %u \n", raw->tx_rs_thresh     );
    printf_depth(depth, "tx_free_thresh    : %u \n", raw->tx_free_thresh   );
    printf_depth(depth, "txq_flags         : %u \n", raw->txq_flags        );
    printf_depth(depth, "tx_deferred_start : %u \n", raw->tx_deferred_start);
    depth--;

    printf_depth(depth, "}\n");
}



inline void print(const struct rte_pci_addr* raw, const char* name="", size_t depth=0)
{
    printf_depth(depth, "%s : %04x:%02x:%02x.%02x\n", name,
            raw->domain, raw->bus, raw->devid, raw->function);
}


inline void print(const struct rte_pci_id* raw, const char* name="", size_t depth=0)
{
    printf_depth(depth, "rte_pci_id %s {\n", name);

    depth++;
    printf_depth(depth, "class_id            : %u \n", raw->class_id           );
    printf_depth(depth, "vendor_id           : %u \n", raw->vendor_id          );
    printf_depth(depth, "device_id           : %u \n", raw->device_id          );
    printf_depth(depth, "subsystem_vendor_id : %u \n", raw->subsystem_vendor_id);
    printf_depth(depth, "subsystem_device_id : %u \n", raw->subsystem_device_id);
    depth--;

    printf_depth(depth, "}\n");
}


inline void print(const struct rte_pci_resource* raw, const char* name="", size_t depth=0)
{
    printf_depth(depth, "rte_pci_resource %s {\n", name);

    depth++;
    printf_depth(depth, "phys_addr : %lu \n", raw->phys_addr);
    printf_depth(depth, "len       : %lu \n", raw->len      );
    printf_depth(depth, "addr      : %p  \n", raw->addr     );
    depth--;

    printf_depth(depth, "}\n");
}


inline void print(const struct rte_epoll_data* raw, const char* name="", size_t depth=0)
{
    printf_depth(depth, "rte_epoll_data %s {\n", name);

    depth++;
    printf_depth(depth, "event  : %u \n", raw->event);
    printf_depth(depth, "data   : %p \n", raw->data );
    printf_depth(depth, "cb_fun : %p \n", raw->cb_fun);
    printf_depth(depth, "cb_arg : %p \n", raw->cb_arg);
    depth--;

    printf_depth(depth, "}\n");
}


inline void print(const struct rte_epoll_event* raw, const char* name="", size_t depth=0)
{
    printf_depth(depth, "rte_epoll_event %s {\n", name);

    depth++;
    printf_depth(depth, "status : %u \n", raw->status);
    printf_depth(depth, "fd     : %u \n", raw->fd    );
    printf_depth(depth, "epfd   : %u \n", raw->epfd  );
    print(&raw->epdata, "rpdata", depth);
    depth--;

    printf_depth(depth, "}\n");
}


inline void print(const struct rte_intr_handle* raw, const char* name="", size_t depth=0)
{
    printf_depth(depth, "rte_intr_handle %s {\n", name);

    depth++;
    printf_depth(depth, "union {\n");
    printf_depth(depth, DEPTHSTR "vfio_dev_fd : %d \n", raw->vfio_dev_fd);
    printf_depth(depth, DEPTHSTR "uio_cfg_fd  : %d \n", raw->uio_cfg_fd );
    printf_depth(depth, "}\n");
    printf_depth(depth, "fd       : %u \n", raw->fd);
    printf_depth(depth, "type     : %u \n", rte_intr_handle_type2str(raw->type));
    printf_depth(depth, "max_intr : %u \n", raw->max_intr);
    printf_depth(depth, "nb_efd   : %u \n", raw->nb_efd  );
    printf_depth(depth, "efds[0-7]     : {%d, %d, %d, %d, %d, %d, %d, %d}\n",
            raw->efds[0], raw->efds[1], raw->efds[2], raw->efds[3],
            raw->efds[4], raw->efds[5], raw->efds[6], raw->efds[7]);
    printf_depth(depth, "efds[8-15]      : {%d, %d, %d, %d, %d, %d, %d, %d}\n",
            raw->efds[ 8], raw->efds[ 9], raw->efds[10], raw->efds[11],
            raw->efds[12], raw->efds[13], raw->efds[14], raw->efds[15]);
    printf_depth(depth, "efds[16-23]     : {%d, %d, %d, %d, %d, %d, %d, %d}\n",
            raw->efds[17], raw->efds[18], raw->efds[19], raw->efds[20],
            raw->efds[21], raw->efds[22], raw->efds[23], raw->efds[24]);
    printf_depth(depth, "efds[24-31]     : {%d, %d, %d, %d, %d, %d, %d, %d}\n",
            raw->efds[24], raw->efds[25], raw->efds[26], raw->efds[27],
            raw->efds[28], raw->efds[29], raw->efds[30], raw->efds[31]);
    print(&raw->elist[0], "elist[0]", depth);
    printf_depth(depth, "elist[1-%zd] {}\n", RTE_MAX_RXTX_INTR_VEC_ID);
    printf_depth(depth, "intr_vec : %p \n", raw->intr_vec);
    depth--;

    printf_depth(depth, "}\n");
}



inline void print(const struct rte_pci_driver* raw, const char* name="", size_t depth=0)
{
    printf_depth(depth, "rte_pci_driver %s {\n", name);

    depth++;
    printf_depth(depth, "next      : %p \n", raw->next);
    printf_depth(depth, "name      : %s \n", raw->name);
    printf_depth(depth, "devinit   : %p \n", raw->devinit  );
    printf_depth(depth, "devuninit : %p \n", raw->devuninit);
    printf_depth(depth, "id_table  : %p \n", raw->id_table);
    printf_depth(depth, "drv_flags : %u \n", raw->drv_flags);
    depth--;

    printf_depth(depth, "}\n");
}




inline void print(const struct rte_devargs* raw, const char* name="", size_t depth=0)
{
    printf_depth(depth, "rte_devargs %s {\n", name);

#if 1
    UNUSED(raw);
#else
    /* unknown sugmentation fault */
    depth++;
    printf_depth(depth, "next : %p \n", raw->next);
    printf_depth(depth, "type : %s \n", rte_devtype2str(raw->type));

    printf_depth(depth, "union {\n");

    printf_depth(depth, DEPTHSTR "struct {\n");
    print(&raw->pci.addr, "addr" , depth);

    printf_depth(depth, DEPTHSTR "} pci\n");
    printf_depth(depth, DEPTHSTR "struct {\n");
    printf_depth(depth, DEPTHSTR "drv_name : %s \n", raw->virt.drv_name);
    printf_depth(depth, DEPTHSTR "} virt\n");
    printf_depth(depth, "}\n");
    printf_depth(depth, "args : %p \n", raw->args);
    depth--;
#endif

    printf_depth(depth, "}\n");
}



inline void print(const struct rte_pci_device* raw, const char* name="", size_t depth=0)
{
    printf_depth(depth, "rte_pci_device %s {\n", name);

    depth++;
    printf_depth(depth, "next : %p \n", raw->next);
    print(&raw->addr           , "addr"           , depth);
    print(&raw->id             , "id"             , depth);
    print(&raw->mem_resource[0], "mem_resource[0]", depth);
    printf_depth(depth, "rte_pci_resource mem_resource[1-%zd]\n", PCI_MAX_RESOURCE);
    print(&raw->intr_handle    , "intr_handle"    , depth);
    print(raw->driver         , "driver"         , depth);
    printf_depth(depth, "max_vfs     : %u \n", raw->max_vfs);
    printf_depth(depth, "numa_node   : %d \n", raw->numa_node);
    print(raw->devargs, "devargs", depth);
    printf_depth(depth, "kdrv        : %s \n", rte_kernel_driver2str(raw->kdrv));
    depth--;

    printf_depth(depth, "}\n");
}


inline void print(const struct rte_eth_dev_info* raw, const char* name="", size_t depth=0)
{
    printf_depth(depth, "rte_eth_dev_info %s {\n", name);

    depth++;
    print(raw->pci_dev, "pci_dev", depth);
    printf_depth(depth, "driver_name            : %s  \n", raw->driver_name);
    printf_depth(depth, "if_index               : %u  \n", raw->if_index);
    printf_depth(depth, "min_rx_bufsize         : %u  \n", raw->min_rx_bufsize        );
    printf_depth(depth, "max_rx_pktlen          : %u  \n", raw->max_rx_pktlen         );
    printf_depth(depth, "max_rx_queues          : %u  \n", raw->max_rx_queues         );
    printf_depth(depth, "max_tx_queues          : %u  \n", raw->max_tx_queues         );
    printf_depth(depth, "max_mac_addrs          : %u  \n", raw->max_mac_addrs         );
    printf_depth(depth, "max_vfs                : %u  \n", raw->max_vfs               );
    printf_depth(depth, "max_vmdq_pools         : %u  \n", raw->max_vmdq_pools        );
    printf_depth(depth, "rx_offload_capa        : %u  \n", raw->rx_offload_capa       );
    printf_depth(depth, "tx_offload_capa        : %u  \n", raw->tx_offload_capa       );
    printf_depth(depth, "reta_size              : %u  \n", raw->reta_size             );
    printf_depth(depth, "hash_key_size          : %u  \n", raw->hash_key_size         );
    printf_depth(depth, "flow_type_rss_offloads : %lu \n", raw->flow_type_rss_offloads);
    print(&raw->default_rxconf, "default_rxconf", depth);
    print(&raw->default_txconf, "default_txconf", depth);
    printf_depth(depth, "vmdq_queue_base        : %u  \n", raw->vmdq_queue_base);
    printf_depth(depth, "vmdq_queue_num         : %u  \n", raw->vmdq_queue_num );
    printf_depth(depth, "vmdq_pool_base         : %u  \n", raw->vmdq_pool_base );
    printf_depth(depth, "rx_desc_lim            : struct rte_eth_desc_lim \n" );
    printf_depth(depth, "tx_desc_lim            : struct rte_eth_desc_lim \n" );
    printf_depth(depth, "speed_capa             : %u  \n", raw->speed_capa  );
    printf_depth(depth, "nb_rx_queues           : %u  \n", raw->nb_rx_queues);
    printf_depth(depth, "nb_tx_queues           : %u  \n", raw->nb_tx_queues);
    depth--;

    printf_depth(depth, "}\n");
}




template<class T>
const char* num2binstr(T num)
{
    size_t bit_length = sizeof(T) * 8;
    static std::string str = "";
    str = "";
    T bit = T(1) << (bit_length-1);
    for (size_t i=0 ;i<bit_length; i++) {
        if (i % 8 == 0)  str += " ";
        if (num & bit) str += "1";
        else           str += "0";

        bit >>= 1;
    }
    return str.c_str();
}



inline void print(const struct rte_eth_rss_reta_entry64* raw, const char* name="", size_t depth=0)
{
    printf_depth(depth, "rte_eth_rss_reta_entry64 %s {\n", name);

    depth++;
    printf_depth(depth, "mask : %lu %s \n", raw->mask, num2binstr<uint64_t>(raw->mask));
    for_i (0, RTE_RETA_GROUP_SIZE) {
        printf_depth(depth, "reta[%2zd] : %5lu %s\n", i, raw->reta[i], num2binstr<uint16_t>(raw->reta[i]));
    }
    depth--;

    printf_depth(depth, "}\n");
}

inline void print(const struct rte_eth_rxq_info* raw, const char* name="", size_t depth=0)
{
    printf_depth(depth, "rte_eth_rxq_info %s {\n", name);

    depth++;
    printf_depth(depth, "mp           : %p \n", raw->mp);
    print(&raw->conf, "conf", depth);
    printf_depth(depth, "scattered_rx : %u \n", raw->scattered_rx);
    printf_depth(depth, "nb_desc      : %u \n", raw->nb_desc);
    depth--;

    printf_depth(depth, "}\n");
}



inline void print(const struct rte_eth_fc_conf* raw, const char* name="", size_t depth=0)
{
    printf_depth(depth, "rte_eth_fc_conf %s {\n", name);

    depth++;
    printf_depth(depth, "high_water         : %u \n", raw->high_water);
    printf_depth(depth, "low_water          : %u \n", raw->low_water );
    printf_depth(depth, "pause_time         : %u \n", raw->pause_time);
    printf_depth(depth, "send_xon           : %u \n", raw->send_xon  );
    printf_depth(depth, "mode               : %s \n", rte_eth_fc_mode2str(raw->mode));
    printf_depth(depth, "mac_ctrl_frame_fwd : %u \n", raw->mac_ctrl_frame_fwd);
    printf_depth(depth, "autoneg            : %u \n", raw->autoneg           );
    depth--;

    printf_depth(depth, "}\n");
}


inline void print(const struct rte_eth_link* raw, const char* name="", size_t depth=0)
{
    printf_depth(depth, "rte_eth_link %s {\n", name);

    depth++;
    printf_depth(depth, "link_speed   : %u \n", raw->link_speed  );
    printf_depth(depth, "link_duplex  : %u \n", raw->link_duplex );
    printf_depth(depth, "link_autoneg : %u \n", raw->link_autoneg);
    printf_depth(depth, "link_status  : %u \n", raw->link_status );
    depth--;

    printf_depth(depth, "}\n");
}


} /* namespace slankdev */


// inline void print(const struct SLANKDEV* raw, const char* name="", size_t depth=0)
// {
//     printf_depth(depth, "SLANKDEV %s {\n", name);
//
//     depth++;
//     printf_depth(depth, " : %u \n", raw->);
//     depth--;
//
//     printf_depth(depth, "}\n");
// }


// inline const char* SLANKDEV 2str(SLANKDEV e)
// {
//     switch (e) {
//         case : return "";
//         case : return "";
//         default: return "UNKNOWN_ERROR";
//     }
// }

