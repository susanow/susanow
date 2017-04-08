#!/bin/sh

PCIADDR1=01:00.1
PCIADDR1=03:00.1

sudo modprobe uio_pci_generic
sudo $RTE_SDK/tools/dpdk-devbind.py -b uio_pci_generic $PCIADDR1
sudo $RTE_SDK/tools/dpdk-devbind.py -b uio_pci_generic $PCIADDR1
sudo $RTE_SDK/tools/dpdk-devbind.py -s
