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
import susanow.d2 as d2

def usage_d2():
    print("Usage: ssnctl d2 [OPTIONS]\n")
    print("OPTIONS")
    print("  deploy <vnfname>      d2 deploy vnf   ")
    print("  out    <vnfname>      d2 opration out ")
    print("  in     <vnfname>      d2 opration in  ")
    print("  check  <vnfname>      check d2 info   ")


def main(argc, argv):
    if (argc < 3):
        usage_d2()
        exit(-1)

    option  = argv[1]
    vnfname = argv[2]
    nfvi = susanow.nfvi.nfvi(os.getenv('SSN_HOST', 'localhost'))
    vnf  = nfvi.get_vnf(vnfname)
    if (vnf == None):
        print('vnf not found')
        exit(-1)

    if   (option == "deploy"):  d2.d2deploy(vnf, nfvi)
    elif (option == "out"   ):  d2.d2out   (vnf, nfvi)
    elif (option == "in"    ):  d2.d2in    (vnf, nfvi)
    elif (option == "check" ):  d2.d2check (vnf, nfvi)
    else:
        print("unknown option '{}'".format(option))
        usage_d2()
        exit(-1)




