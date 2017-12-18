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
from . import port
from . import ppp


class Vnf:
    _nfvi = ''
    _name = ''
    _this = ''

    def __init__(self, name, nfvi):
        self._name = name
        self._nfvi = nfvi
        self.sync()

    def sync(self):
        res = self._nfvi._get('/vnfs/' + self._name)
        if (res['result']['success']):
            self._this = res['vnf']
        else:
            raise Exception('vnf not found {}'.format(self._name))

    def name    (self): return self._name
    def running (self): return self._this['running' ]
    def coremask(self): return self._this['coremask']
    def n_port  (self): return self._this['n_port'  ]
    def n_block (self): return self._this['n_block' ]
    def perfred (self): return self._this['perfred' ]
    def rxrate  (self): return self._this['rxrate'  ]

    def block(self, bid):
        blk = Block(self.name(), bid, self._nfvi)
        return blk

    def port(self, pid):
        port_json = self._this['ports'][str(pid)]
        if (port_json == 'nil'):
            return None
        name = port_json['name']
        port0 = port.Port(name, self._nfvi)
        return port0

    def ports(self):
        ports = []
        for pid in range(self.n_port()):
            port = self.port(pid)
            ports.append(port)
        return ports

    def blocks(self):
        blocks = []
        for bid in range(self.n_block()):
            block = self.block(bid)
            blocks.append(block)
        return blocks

    def show(self):
        self.sync()
        print('name     : {}'.format(self.name()))
        run = self.running ()
        print('running  : {}'.format(run))
        print('coremask : {}'.format(self.coremask()))
        print('n_port   : {}'.format(self.n_port  ()))
        print('n_block  : {}'.format(self.n_block ()))
        if (run):
            print('perfred  : {}'.format(self.perfred ()))
            print('rxrate   : {}'.format(self.rxrate  ()))
        np = self.n_port()
        for pid in range(np):
            print('port[{}]: {{'.format(pid), end='')
            port = self.port(pid)
            if (port == None): print('nil');
            else:
                print('')
                port.show()
            print('}')
        nb = self.n_block()
        for bid in range(nb):
            print('block[{}] {{'.format(bid), end='')
            print("")
            block = self.block(bid)
            block.show()
            print('}')

    def set_coremask(self, bid, mask):
        data  = '{\n'
        data += '   \"coremask\" : {}\n'.format(mask)
        data += '}\n'
        uri = '/vnfs/{}/coremask/{}'.format(self._name, bid)
        res = self._nfvi._put(uri, data)
        if (res['result']['success'] == False):
            raise Exception('Could\'nt set ({})'.format(res['result']['msg']))

    def reset(self):
        uri = '/vnfs/{}/reset'.format(self._name)
        res = self._nfvi._put(uri, '')
        if (res['result']['success'] == False):
            raise Exception('Could\'nt reset ({})'.format(res['result']['msg']))

    def deployable(self):
        res = self._nfvi._get('/vnfs/{}'.format(self._name))
        return res['vnf']['deployable']

    def deploy(self):
        res = self._nfvi._put('/vnfs/{}/deploy'.format(self._name), '')
        if (res['result']['success'] == False):
            msg = res['result']['msg']
            raise Exception('Miss: {}'.format(msg))

    def undeploy(self):
        uri = '/vnfs/{}/undeploy'.format(self._name)
        res = self._nfvi._put(uri, '')
        if (res['result']['success'] == False):
            msg = res['result']['msg']
            raise Exception('Miss: {}'.format(msg))

    def attach_port(self, pid, port):
        vname = self.name()
        pname = port.name()
        uri = '/vnfs/' + vname + '/ports/' + str(pid)
        data  = '{\n'
        data += '   \"pname\" : \"{}\"\n'.format(pname)
        data += '}\n'
        res = self._nfvi._put(uri, data)
        if (res['result']['success'] == False):
            msg = res['result']['msg']
            raise Exception('Miss: {}'.format(msg))

    def detach_port(self, pid):
        vname = self.name()
        uri = '/vnfs/' + vname + '/ports/' + str(pid)
        res = self._nfvi._delete(uri)
        if (res['result']['success'] == False):
            msg = res['result']['msg']
            raise Exception('Miss: {}'.format(msg))

class Block:
    _vnfname = ''
    _vnf_bid = ''
    _nfvi = ''
    _this = ''

    def __init__(self, vnfname, bid, nfvi):
        self._vnfname = vnfname
        self._vnf_bid = bid
        self._nfvi = nfvi
        self.sync()

    def sync(self):
        res = self._nfvi._get('/vnfs/' + self._vnfname)
        if (res['result']['success']):
            self._this = res['vnf']['blocks'][str(self._vnf_bid)]
        else:
            raise Exception('block not found {}'.format(self._name))

    def name    (self): return self._this['name']
    def bid     (self): return self._vnf_bid
    def coremask(self): return self._this['coremask']
    def running (self): return self._this['running']
    def socket_affinity(self): return self._this['socket_affinity']

    def show(self):
        self.sync()
        print('name            : {}'.format(self.name()))
        print('coremask        : {}'.format(self.coremask()))
        print('running         : {}'.format(self.running()))
        print('socket_affinity : {}'.format(self.socket_affinity()))


