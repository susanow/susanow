#!/usr/bin/env python3
#
# MIT License
# Copyright (c) 2017 Susanow
# Copyright (c) 2017 Hiroki SHIROKURA
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

import time
import libssnctl.ssnctl as ssnctl
VNFNAME='vnf0'
THRESHOLD=0.8

def main():
    vnf = ssnctl.get_vnf('vnf0')
    cnt = 0
    while True:
        cnt = cnt + 1
        pr = vnf.get_perfred()
        rate = vnf.get_rx_rate()
        if (pr < THRESHOLD):
            vnf.d2op_out()
            print('d2 operation out, wait...')
            time.sleep(5)
            print('d2 operation out fin')
        print('Loop {} rxrate={} reduction={}'.format(cnt,rate,pr))
        time.sleep(1)

if __name__ == '__main__':
    main()

