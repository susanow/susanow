#!/usr/bin/env python3

def watch_vnf(vnfname):
    import time
    import susanow
    nfvi = susanow.nfvi.nfvi()
    vnf = nfvi.get_vnf(vnfname)
    if (vnf == None): exit(-1)
    while (True):
        vnf.sync()
        rate = vnf.rxrate()
        perf = vnf.perfred() * 100
        print('{}:  {:.0f}pps  {:.1f}%'.format(vnfname, rate, perf))
        time.sleep(1)

def main():
    import sys
    argc = len(sys.argv)
    argv = sys.argv
    if (argc < 2):
        print("Usage: {} <vnfname>".format(argv[0]))
        exit(-1)
    vnfname = argv[1]
    watch_vnf(vnfname)

if __name__ == '__main__':
    main()

