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
import time
import susanow

def usage_nfvi():
    print("Usage: ssnctl nfvi [OPTIONS]")
    print("OPTIONS")
    print("  stat      nfvi stat")
    print("  ping      ping to nfvi")
    print("  pnic      physical nics")
    print("  cpu       cpu info")
    print("  drv       show nic and drivers")

def main(argc, argv):
    if (argc < 2):
        usage_nfvi()
        exit(-1)
    nfvi = susanow.nfvi.nfvi(os.getenv('SSN_HOST', 'localhost'))
    option = argv[1]
    if (option == "stat"):
        nfvi_stat(nfvi)
        return
    if (option == "cpu"):
        nfvi_cpu(nfvi)
        return
    if (option == "pnic"):
        nfvi_pnic(nfvi)
        return
    if (option == "ping"):
        nfvi_ping(nfvi)
        return
    if (option == "drv"):
        nfvi_drv(nfvi)
        return
    usage_nfvi()
    exit(-1)

def nfvi_pnic(nfvi):
    import requests
    url =  'http://' + nfvi._host + ':8888' + '/system/pnic'
    json = requests.get(url).json()
    n_pnic = json['n_pnic']
    for i in range(n_pnic):
        pnic = json[str(i)]
        print("port{}".format(pnic['port_id']))
        rx_pps = pnic['cur_rx_pps']
        rx_mis = pnic['cur_rx_mis']
        tx_pps = pnic['cur_tx_pps']
        print(" cur rx_pkt: {}".format(rx_pps))
        print(" cur rx_mis: {}".format(rx_mis))
        print(" cur tx_pkt: {}".format(tx_pps))
        print(" tot rx_pkt: {}".format(pnic['tot_ipackets']))
        print(" tot tx_pkt: {}".format(pnic['tot_opackets']))
    return

def nfvi_cpu(nfvi):
    import requests
    url =  'http://' + nfvi._host + ':8888' + '/system/cpu'
    json = requests.get(url).json()
    print('{:3} {:3} {:8} {:4}'.format('soc', 'lid', 'state', 'rate'))
    n_cpu = json['n_cpu']
    for i in range(n_cpu):
        cpu = json[str(i)]
        n_core = cpu['n_core']
        print("--- --- -------- ----")
        for j in range(n_core):
            core = cpu[str(j)]
            print('{:03} {:03} {:<8} {:03}%'.format(
                core['socket_id'],
                core['lcore_id'],
                core['state'],
                core['usage_rate']))
    print("--- --- -------- ----")
    return

def nfvi_drv(nfvi):
    devices = nfvi.get_nic_details()
    devices = sorted(devices.items())
    for d in devices:
        dev     = d[1]
        slot    = dev['Slot']
        detail  = dev['SDevice_str']
        driver  = dev['Driver_str'] if ('Driver_str' in dev) else '<none>'
        module  = dev['Module_str']
        iface   = dev['Interface'] if (dev['Interface'] != '') else '<none>'
        dpdk_en = 'y' if (driver in nfvi.dpdk_drivers) else 'n'
        print("{:15} {:50} {:5} {:10} {:20} {:40}"
                .format(slot, detail, dpdk_en, iface, driver, module))

def nfvi_ping(nfvi):
    while (True):
        try:
            delta = nfvi.ping()
            print('ssn-ping from {}:{} time={:.3f}'.format(nfvi.host(), nfvi.port(), delta))
            time.sleep(1)
        except:
            return

def nfvi_stat(nfvi):
    nfvi.show()




