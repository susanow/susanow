#!/usr/bin/env python3
# -*- coding: utf-8 -*-
#
# MIT License
# Copyright (c) 2017 Susanow
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.

import os
import susanow

def usage_port():
    print("Usage: ssnctl port [OPTIONS]\n")
    print("OPTIONS")
    print("  list                              show ports list       ")
    print("  stat   <name>                     get port status       ")
    print("  alloc  <name> pci <0000:01:00.0>  allocate new pci port ")
    print("  alloc  <name> tap <tap0>          allocate new tap port ")
    print("  alloc  <name> virt                allocate new virt port")
    print("  delete <name>                     delete port           ")

def main(argc, argv):
    if (argc < 2):
        usage_port()
        exit(-1)
    nfvi = susanow.nfvi.nfvi(os.getenv('SSN_HOST', 'localhost'))

    option = argv[1]
    if (option == "list"):
        port_list(nfvi)
        return

    if (argc < 3):
        usage_port()
        exit(-1)
    pname  = argv[2]
    if   (option == "stat"  ):
        port_NAME_stat(pname, nfvi)
        return
    elif (option == "delete"):
        port_NAME_delete(pname, nfvi)
        return
    elif (option == "alloc" ):
        if (argc >= 4):
            subop = argv[3]
            if   (subop == "pci"):
                if (argc >= 5):
                    pciaddr = argv[4]
                    port_NAME_alloc_pci(pname, pciaddr, nfvi)
                    return
            elif (subop == "tap"):
                if (argc >= 5):
                    ifname = argv[4]
                    port_NAME_alloc_tap(pname, ifname, nfvi)
                    return
            elif (subop == "virt"):
                port_NAME_alloc_virt(pname, nfvi)
                return
    usage_port()
    exit(-1)

def port_list(nfvi):
    ports = nfvi.list_ports()
    idx = 0
    for p in ports:
        p.sync()
        n = p.name()
        nrxq = p.n_rxq()
        ntxq = p.n_txq()
        nrxa = p.n_rxa()
        ntxa = p.n_txa()
        a    = p.attach()
        if (a != None): a = a.name()
        print('[{}] n={} nrxq={} ntxq={} nrxa={} ntxa={} a={}'
                .format(idx, n, nrxq, ntxq, nrxa, ntxa, a))
        idx = idx + 1

def port_NAME_stat(pname, nfvi):
    port = nfvi.get_port(pname)
    if (port == False):
        print('not found')
        exit(-1)
    port.show()

def port_NAME_delete(pname, nfvi):
    nfvi.delete_port(pname)

def port_NAME_alloc_pci(pname, pciaddr, nfvi):
    port = nfvi.alloc_port_pci(pname, pciaddr)

def port_NAME_alloc_tap(pname, ifname, nfvi):
    port = nfvi.alloc_port_tap(pname, ifname)

def port_NAME_alloc_virt(pname, nfvi):
    port = nfvi.alloc_port_virt(pname)


