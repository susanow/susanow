

DPDK_PATH        = $(SSN_SDK)/dpdk/x86_64-native-linuxapp-gcc
LTHREAD_PATH     = $(SSN_SDK)/liblthread_dpdk
LIBVTY_PATH      = $(SSN_SDK)/libvty
LIBSLANKDEV_PATH = $(SSN_SDK)/libslankdev
LIBDPDK_CPP_PATH = $(SSN_SDK)/libdpdk_cpp
LIBSSN_PATH      = $(SSN_SDK)/libsusanow

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

SSN_LIB_CXXFLAGS += \
		-I$(DPDK_PATH)/include \
		-I$(LTHREAD_PATH)      \
		-I$(LIBVTY_PATH)       \
		-I$(LIBSLANKDEV_PATH)  \
		-I$(LIBDPDK_CPP_PATH)  \
		-I$(LIBSSN_PATH)       \
		-m64 -mssse3           \
		-std=c++11

SSN_LIB_LDFLAGS  += \
	  -L$(DPDK_PATH)/lib         \
	  -L$(LTHREAD_PATH)          \
		-L$(LIBVTY_PATH)           \
		-L$(LIBSSN_PATH)           \
		-llthread_dpdk \
		-lvty          \
		$(DPDK_LDFLAGS)

SSN_CXXFLAGS =  $(SSN_LIB_CXXFLAGS)
SSN_LDFLAGS  = -lsusanow $(SSN_LIB_LDFLAGS)


