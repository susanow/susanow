



run: run_native

run_native:
	sudo ./a.out

run_dummy:
	sudo ./a.out   \
		--vdev=eth_null0 \
		--vdev=eth_null1

run_vagrant:
	sudo ./a.out   \
		--vdev=eth_pcap0,iface=vboxnet2 \
		--vdev=eth_pcap1,iface=lo

re: clean all

gdb:
	sudo gdb \
		--args a.out  \
		--vdev=eth_null0 \
		--vdev=eth_null1

		# --vdev=eth_null0  \
		# --vdev=eth_null1

		# --vdev=eth_pcap0,iface=vboxnet2 \
		# --vdev=eth_pcap1,iface=vboxnet3
		# --args a.out \
		# --args a.out

# run:
# 	sudo ./a.out \
# 		--vdev=eth_pcap0,iface=lo
#
# gdb:
# 	sudo gdb --args a.out \
# 		--vdev=eth_pcap0,iface=lo \
# 		--vdev=eth_null3
