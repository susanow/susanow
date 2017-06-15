

DPDK_PATH := $(RTE_SDK)/$(RTE_TARGET)

DPDK_CFLAGS = \
	 -I$(DPDK_PATH)/include \
	 -include $(DPDK_PATH)/include/rte_config.h \
	 -mssse3

DPDK_LDFLAGS += \
	-Wl,--no-as-needed \
	-Wl,-export-dynamic \
	-L$(DPDK_PATH)/lib \
	-lpthread -ldl -lrt -lm -lpcap \
	-Wl,--whole-archive -Wl,--start-group \
	-lrte_jobstats -lrte_pipeline -lrte_pmd_ixgbe -lrte_pmd_thunderx_nicvf \
	-lrte_acl -lrte_kni -lrte_pmd_af_packet -lrte_pmd_kni -lrte_pmd_vhost \
	-lrte_bitratestats -lrte_kvargs -lrte_pmd_ark -lrte_pmd_lio -lrte_pmd_virtio \
	-lrte_cfgfile -lrte_latencystats -lrte_pmd_avp -lrte_pmd_nfp -lrte_pmd_vmxnet3_uio \
	-lrte_cmdline -lrte_lpm -lrte_pmd_bnxt -lrte_pmd_null -lrte_port \
	-lrte_cryptodev -lrte_mbuf -lrte_pmd_bond -lrte_pmd_null_crypto -lrte_power \
	-lrte_distributor -lrte_mempool -lrte_pmd_crypto_scheduler -lrte_pmd_octeontx_ssovf -lrte_reorder \
	-lrte_eal -lrte_mempool_ring -lrte_pmd_cxgbe -lrte_pmd_qede -lrte_ring \
	-lrte_efd -lrte_mempool_stack -lrte_pmd_e1000 -lrte_pmd_ring -lrte_sched \
	-lrte_ethdev -lrte_meter -lrte_pmd_ena -lrte_pmd_sfc_efx -lrte_table \
	-lrte_eventdev -lrte_metrics -lrte_pmd_enic -lrte_pmd_skeleton_event -lrte_timer \
	-lrte_hash -lrte_net -lrte_pmd_fm10k -lrte_pmd_sw_event -lrte_vhost \
	-lrte_ip_frag -lrte_pdump -lrte_pmd_i40e -lrte_pmd_tap \
	-Wl,--end-group -Wl,--no-whole-archive
