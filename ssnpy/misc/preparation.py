#!/usr/bin/env python3

import susanow
import susanow.d2 as d2

nfvi = susanow.nfvi.nfvi()
vnf = nfvi.alloc_vnf('vnf0', 'l2fwd2b')
pci0 = nfvi.alloc_port_pci('pci0', '0000:3b:00.1')
pci1 = nfvi.alloc_port_pci('pci1', '0000:5f:00.1')
if (vnf == None or pci0 == None or pci1 == None):
    print("error")
    exit(-1)

vnf.attach_port(0, pci0)
vnf.attach_port(1, pci1)
vnf.reset()
d2.d2deploy(vnf, nfvi)

