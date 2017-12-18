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

def usage_vnf():
    print("Usage: ssnctl vnf [OPTIONS]\n")
    print("OPTIONS")
    print("  list                                   show vnf list             ")
    print("  stat        <name>                     get vnf status            ")
    print("  alloc       <name> <cname>             allocate new vnf          ")
    print("  delete      <name>                     delete vnf                ")
    print("  attachport  <name> <pid> <pname>       attach port to vnf        ")
    print("  detachport  <name> <pid>               detach port from vnf      ")
    print("  deploy      <name>                     deploy vnf                ")
    print("  undeploy    <name>                     undeploy vnf              ")
    print("  reset       <name>                     reset vnf's accessor infos")
    print("  setcoremask <name> <bid> <coremask>    set block's coremask      ")

def main(argc, argv):
    if (argc < 2):
        usage_vnf()
        exit(-1)
    nfvi = susanow.nfvi.nfvi(os.getenv('SSN_HOST', 'localhost'))

    option   = argv[1]
    if (option == "list"):
        vnf_list(nfvi)
        return

    if (argc < 3):
        usage_vnf()
        exit(-1)
    vnfname  = argv[2]

    if (option == "stat"      ):
        vnf_NAME_stat(vnfname, nfvi)
        return
    elif (option == "alloc"     ):
        if (argc >= 4):
            cname = argv[3]
            vnf_NAME_alloc_CNAME(vnfname, cname, nfvi)
            return
    elif (option == "delete"    ):
        vnf_NAME_delete(vnfname, nfvi)
        return
    elif (option == "attachport"):
        if (argc >= 5):
            pid   = argv[3]
            pname = argv[4]
            vnf_NAME_attachport_PID_PNAME(vnfname, pid, pname, nfvi)
            return
    elif (option == "detachport"):
        if (argc >= 4):
            pid   = argv[3]
            vnf_NAME_detachport_PID(vnfname, pid, nfvi)
            return
    elif (option == "reset"     ):
        vnf_NAME_reset(vnfname, nfvi)
        return
    elif (option == "setcoremask"):
        if (argc >= 5):
            bid   = argv[3]
            coremask = argv[4]
            vnf_NAME_setcoremask_BID_COREMASK(vnfname, bid, coremask, nfvi)
            return
    elif (option == "deploy"    ):
        vnf_NAME_deploy(vnfname, nfvi)
        return
    elif (option == "undeploy"  ):
        vnf_NAME_undeploy(vnfname, nfvi)
        return
    usage_vnf()
    exit(-1)

def vnf_list(nfvi):
    vnfs = nfvi.list_vnfs()
    idx = 0
    for vnf in vnfs:
        n = vnf.name()
        p = vnf.n_port()
        b = vnf.n_block()
        r = vnf.running()
        c = vnf.coremask()
        print('[{}] n={} p={} b={} r={} c={}'.format(idx, n, p, b, r, c))
        idx = idx + 1

def vnf_NAME_stat(vnfname, nfvi):
    vnf0 = nfvi.get_vnf(vnfname)
    if (vnf0 == None):
        print('not found')
        exit(-1)
    vnf0.show()

def vnf_NAME_deploy(vnfname, nfvi):
    vnf0 = nfvi.get_vnf(vnfname)
    if (vnf0 == None):
        print('not found')
        exit(-1)
    vnf0.deploy()

def vnf_NAME_undeploy(vname, nfvi):
    vnf0 = nfvi.get_vnf(vname)
    if (vnf0 == None):
        print('not found')
        exit(-1)
    vnf0.undeploy()

def vnf_NAME_alloc_CNAME(iname, cname, nfvi):
    nfvi.alloc_vnf(iname, cname)

def vnf_NAME_delete(vnfname, nfvi):
    nfvi.delete_vnf(vnfname)

def vnf_NAME_attachport_PID_PNAME(vname, pid, pname, nfvi):
    vnf0 = nfvi.get_vnf(vname)
    if (vnf0 == None):
        print('Not found {}'.format(vname))
        exit(-1)
    port = nfvi.get_port(pname)
    if (port == None):
        print('Not found {}'.format(pname))
        exit(-1)
    vnf0.attach_port(pid, port)

def vnf_NAME_detachport_PID(vname, pid, nfvi):
    vnf0 = nfvi.get_vnf(vname)
    if (vnf0 == None):
        print('Not found {}'.format(vname))
        exit(-1)
    vnf0.detach_port(pid)

def vnf_NAME_reset(vname, nfvi):
    vnf0 = nfvi.get_vnf(vname)
    if (vnf0 == None):
        print('not found')
        exit(-1)
    vnf0.reset()

def vnf_NAME_setcoremask_BID_COREMASK(vname, bid, coremask, nfvi):
    vnf0 = nfvi.get_vnf(vname)
    if (vnf0 == None):
        print('not found')
        exit(-1)
    vnf0.set_coremask(bid, coremask)


