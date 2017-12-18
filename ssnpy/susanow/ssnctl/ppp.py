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

def usage_ppp():
    print("Usage: ssnctl ppp [OPTIONS]\n")
    print("OPTIONS")
    print("  list                              show ppp's list         ")
    print("  stat   <name>                     get ppp's status        ")
    print("  alloc  <name> <left> <right>      allocate new patch-panel")
    print("  delete <name>                     delete ppp              ")

def main (argc, argv):
    if (argc < 2):
        usage_ppp()
        exit(-1)

    nfvi = susanow.nfvi.nfvi(os.getenv('SSN_HOME', 'localhost'))
    option = argv[1]
    if (option == 'list'):
        ppp_list(nfvi)
        return

    if (argc < 3):
        usage_ppp()
        exit(-1)
    name   = argv[2]
    if (option == "stat"):
        ppp_NAME_stat(name, nfvi)
        return
    elif (option == "delete"):
        ppp_NAME_delete(name, nfvi)
        return
    elif (option == "alloc"):
        if (argc >= 5):
            left  = argv[3]
            right = argv[4]
            ppp_NAME_alloc(name, left, right, nfvi)
            return
    usage_ppp()
    exit(-1)

def ppp_list(nfvi):
    ppps = nfvi.list_ppps()
    idx = 0
    for ppp in ppps:
        n = ppp.name()
        print('[{}] n={}'.format(idx, n))
        idx = idx + 1


def ppp_NAME_stat(name, nfvi):
    ppp = nfvi.get_ppp(name)
    if (ppp == False):
        print('not found')
        exit(-1)
    ppp.show()

def ppp_NAME_delete(name, nfvi):
    nfvi.delete_ppp(name)

def ppp_NAME_alloc(name, left_name, right_name, nfvi):
    l_port = nfvi.get_port(left_name)
    r_port = nfvi.get_port(right_name)
    if (l_port == None or r_port == None):
        print('invalid port name')
        exit(-1)
    ppp = nfvi.alloc_ppp(name, l_port, r_port)



