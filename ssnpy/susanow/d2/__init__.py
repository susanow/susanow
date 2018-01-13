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


import susanow


def d2deploy(vnf, nfvi):
    block_coremasks = _get_nxt_d2out_coremasks(vnf, nfvi)
    vnf.reset()
    for bid in range(vnf.n_block()):
        coremask = block_coremasks[bid]
        assert type(coremask) == type(0x3)
        vnf.set_coremask(bid, coremask)
    vnf.deploy()


def d2out(vnf, nfvi):
    block_coremasks = _get_nxt_d2out_coremasks(vnf, nfvi)
    vnf.undeploy()
    vnf.reset()
    for bid in range(vnf.n_block()):
        coremask = block_coremasks[bid]
        assert type(coremask) == type(0x3)
        vnf.set_coremask(bid, coremask)
    vnf.deploy()


def d2in(vnf, nfvi):
    block_coremasks = _get_nxt_d2in_coremasks(vnf, nfvi)
    vnf.undeploy()
    vnf.reset()
    for bid in range(vnf.n_block()):
        coremask = block_coremasks[bid]
        assert type(coremask) == type(0x3)
        vnf.set_coremask(bid, coremask)
    vnf.deploy()


def d2check(vnf, nfvi):
    curr = []
    for bid in range(vnf.n_block()):
        curr.append(vnf.block(bid).coremask())
    nxto = _get_nxt_d2out_coremasks(vnf, nfvi)
    nxti = _get_nxt_d2in_coremasks(vnf, nfvi)
    print('in: ')
    for bid in range(vnf.n_block()):
        print(' block{}: {:5}  {}'
            .format(bid, nxti[bid], format(nxti[bid], '032b')))
    print('current:')
    for bid in range(vnf.n_block()):
        print(' block{}: {:5}  {}'
            .format(bid, curr[bid], format(curr[bid], '032b')))
    print('out: ')
    for bid in range(vnf.n_block()):
        print(' block{}: {:5}  {}'
            .format(bid, nxto[bid], format(nxto[bid], '032b')))


def _popcnt(bitmask):
    count = 0
    pivot = 0x1
    while (bitmask != 0):
        count = count + (bitmask & pivot)
        bitmask = bitmask >> 1
    return count


def _mask2array32(mask):
    array = []
    pivot = 0x1
    for i in range(16):
        try:
            array.append(mask & pivot)
            mask = mask >> 1
        except:
            print('mask : ', mask)
            print('pivot: ', pivot)
            print('okashii!')
    return array


def _get_sys_ava_cores(vnf, nfvi):
    cores = nfvi.get_cores()
    sys_ava = []
    for core in cores:
       if (core['state'] == 'WAIT'): sys_ava.append(1)
       else: sys_ava.append(0)
    vnf_cur = _mask2array32(vnf.coremask())
    res = []
    for i in range(16):
        n = sys_ava[i] | vnf_cur[i]
        res.append(n)
    return res


def _get_vnf_ava_cores(vnf, nfvi, bid):
    cores = nfvi.get_cores()
    array = []
    for core in cores:
        b = vnf.block(bid)
        sa = b.socket_affinity()
        if (core['socket_id'] == sa): array.append(1)
        else: array.append(0)
    return array


def _get_block_ava_cores(vnf, nfvi, bid):
    sys_ava = _get_sys_ava_cores(vnf, nfvi)
    vnf_cur = _mask2array32(vnf.block(bid).coremask())
    vnf_ava = _get_vnf_ava_cores(vnf, nfvi, bid)
    res = []
    for i in range(16):
        n = (sys_ava[i] | vnf_cur[i]) & vnf_ava[i]
        res.append(n)
    return res


def _get_nxt_coremask_ncore(vnf, nfvi, n_cores):
    sys_ava = _get_sys_ava_cores(vnf, nfvi)
    block_ava = []
    for i in range(vnf.n_block()):
        block_ava.append(_get_block_ava_cores(vnf, nfvi, i))

    block_coremasks = []
    blocks = vnf.blocks()
    for block in blocks:
        bid = block.bid()
        for i in range(16):
            block_ava[bid][i] = block_ava[bid][i] & sys_ava[i]

        nxt_n_core = n_cores

        enough = False
        cnt = 0
        nxt_coremask = 0
        for i in range(16):
            if (block_ava[bid][i] == 1):
                nxt_coremask = nxt_coremask + (0x1 << i)
                block_ava[bid][i] = 0
                sys_ava[i] = 0
                cnt = cnt + 1
                if (cnt == nxt_n_core):
                    enough = True
                    break
        if (enough == False):
            # no enough cores
            return None
        block_coremasks.append(nxt_coremask)
    return block_coremasks


def _get_nxt_d2out_coremasks(vnf, nfvi):
    cur_coremask = vnf.block(0).coremask()
    cur_n_core = _popcnt(cur_coremask)
    nxt_n_core = 1 if (cur_n_core==0) else cur_n_core*2
    if (nxt_n_core > 8): nxt_n_core = cur_n_core
    ret = _get_nxt_coremask_ncore(vnf, nfvi, nxt_n_core)
    if (ret == None): return _get_nxt_coremask_ncore(vnf, nfvi, cur_n_core) # error occurs
    else: return ret


def _get_nxt_d2in_coremasks(vnf, nfvi):
    cur_coremask = vnf.block(0).coremask()
    cur_n_core = _popcnt(cur_coremask)
    nxt_n_core = 1 if (cur_n_core==1) else cur_n_core/2
    ret = _get_nxt_coremask_ncore(vnf, nfvi, nxt_n_core)
    if (ret == None): return _get_nxt_coremask_ncore(vnf, nfvi, cur_n_core) # error occurs
    else: return ret



