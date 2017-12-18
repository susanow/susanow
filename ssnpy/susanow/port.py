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


from . import nfvi
from . import vnf
from . import ppp


class Port:
    _nfvi = ''
    _name = ''
    _this = ''

    def __init__(self, name, nfvi):
        self._name = name
        self._nfvi = nfvi
        self.sync()

    def sync(self):
        res = self._nfvi._get('/ports/' + self._name)
        if (res['result']['success']):
            self._this = res['port']
        else:
            raise Exception('port not found {}'.format(self._name))

    def name (self)    : return self._name
    def n_rxq(self)    : return self._this['n_rxq']
    def n_txq(self)    : return self._this['n_txq']
    def n_rxa(self)    : return self._this['n_rxa']
    def n_txa(self)    : return self._this['n_txa']
    def outer_rxp(self): return self._this['outer_rxp']
    def outer_txp(self): return self._this['outer_txp']
    def inner_rxp(self): return self._this['inner_rxp']
    def inner_txp(self): return self._this['inner_txp']
    def socket_id(self): return self._this['socket_id']

    def attach(self):
        ret = self._this['attach']
        if (ret == 'nill'): return None
        else:
            vname = ret
            vnf = self._nfvi.get_vnf(vname)
            return vnf

    def show(self):
        self.sync()
        print('name      : {}'.format(self.name()))
        print('n_rxq     : {}'.format(self.n_rxq()))
        print('n_txq     : {}'.format(self.n_txq()))
        print('n_rxa     : {}'.format(self.n_rxa()))
        print('n_txa     : {}'.format(self.n_txa()))
        print("outer_rxp : {}".format(self.outer_rxp()))
        print("outer_txp : {}".format(self.outer_txp()))
        print("inner_rxp : {}".format(self.inner_rxp()))
        print("inner_txp : {}".format(self.inner_txp()))
        print("attach    : {}".format(self.attach()))
        print("socket_id : {}".format(self.socket_id()))


