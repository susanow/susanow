#!/usr/bin/env python3

import sys
import math
import time
import threading
import susanow
import susanow.d2 as d2

def d2out_ignition(vnf, nfvi, threshold):
    perf = vnf.perfred() * 100
    return (perf < threshold)

def d2in_ignition(vnf, nfvi, data):
    perf = vnf.perfred()
    rate = vnf.rxrate()
    p = math.floor((rate * perf) * (rate * perf))

    data_ = data
    diff = data[-1]['a'] / data[-5]['a']
    if (diff < 0.7):
        return True
    return False

def data_getter(vnf, data):
    while (True):
        vnf.sync()

        rate = vnf.rxrate()
        perf = vnf.perfred()
        perf = 1.0 if (perf>1.0) else perf
        p = math.floor((rate * perf) * (rate * perf))

        a = p
        for i in range(4):
            a = a + data[-1 * i]['p']
        d = { 'p':p, 'a':a }
        data.append(d)

        if (len(data) > 20): data.pop(0)

        # for i in range(5):
        #     print('{}'.format(data[-6 + i]['a']))
        # print('-----------------------')

        time.sleep(0.5)


def d2agent(vnf, nfvi, data):
    while (True):
        vnf.sync()

        if (d2out_ignition(vnf, nfvi, 60)):
            print("need d2out")
            d2.d2out(vnf, nfvi)
            time.sleep(5)

        if (d2in_ignition(vnf, nfvi, data)):
            print('need d2in')
            d2.d2in(vnf, nfvi)
            time.sleep(5)

        time.sleep(0.5)


def main():
    argc = len(sys.argv)
    argv = sys.argv
    if (argc < 2):
        print("Usage: {} <vnfname>".format(argv[0]))
        exit(-1)

    nfvi = susanow.nfvi.nfvi()
    vnf = nfvi.get_vnf(argv[1])
    if (vnf == None):
        print('vnf not found')
        exit(-1)

    data = []
    d = { 'p':1, 'a':1 }
    data.append(d)
    data.append(d)
    data.append(d)
    data.append(d)
    data.append(d)

    t0 = threading.Thread(target=data_getter, args=(vnf, data))
    t0.start()
    d2agent(vnf, nfvi, data)

if __name__ == '__main__':
    main()

