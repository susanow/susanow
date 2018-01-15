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


import json
import time
import sys
import pprint
import requests
import subprocess
import os
from os.path import exists
from os.path import abspath
from os.path import dirname
from os.path import basename

from . import vnf
from . import port
from . import ppp

def _check_output(args, stderr=None):
    return subprocess.Popen(args, stdout=subprocess.PIPE,
                            stderr=stderr).communicate()[0]


class nfvi:
    _host = ''
    _port = ''
    _this = ''
    ETHERNET_CLASS = "0200"
    dpdk_drivers = [ "igb_uio", "vfio-pci", "uio_pci_generic" ]

    def n_core(self): return self._this['n_core']
    def n_socket(self): return self._this['n_socket']
    def cores(self): return self._this['cores']

    def get_cores(self):
        cores_j = self.cores()
        n_core = self.n_core()
        cores = []
        for i in range(n_core):
            core = {}
            core['lcore_id'] = cores_j[str(i)]['lcore_id']
            core['socket_id'] = cores_j[str(i)]['socket_id']
            core['state'] = cores_j[str(i)]['state']
            cores.append(core)
        return cores

    def get_noava_cpus(self):
        dellist = lambda items, indexes: [ \
            item for index, item in enumerate(items) \
                if index not in indexes ]
        cpus = self.get_cpus()
        for cpu in cpus:
            dlist = []
            idx = 0
            for core in cpu['cores']:
                if (core['state'] == "WAIT"):
                    dlist.append(idx)
                idx = idx + 1
            cpu['cores'] = dellist(cpu['cores'], dlist)
        return cpus

    def get_ava_cpus(self):
        dellist = lambda items, indexes: [ \
            item for index, item in enumerate(items) \
                if index not in indexes ]
        cpus = self.get_cpus()
        for cpu in cpus:
            dlist = []
            idx = 0
            for core in cpu['cores']:
                if (core['state'] != "WAIT"):
                    dlist.append(idx)
                idx = idx + 1
            cpu['cores'] = dellist(cpu['cores'], dlist)
        return cpus

    def get_cpus(self):
        cores = self.get_cores()
        cpus = []
        n_sock = self.n_socket()
        for i in range(n_sock):
            cpu = {}
            cpu['socket_id'] = i
            cpu['cores'] = []
            for c in cores:
                if (int(c['socket_id']) == i):
                    cpu['cores'].append(c)
            cpus.append(cpu)
        return cpus

    def show(self):
        import requests
        url =  'http://' + self._host + ':8888' + '/system'
        json = requests.get(url).json()
        print("host       : {}".format(self._host))
        print("apiport    : {} (#hardcode)".format(8888))
        print("n_vnf      : {}".format(json['n_vnf']))
        print("n_ports    : {}".format(json['n_port']))
        print("n_socket   : {}".format(json['nfvi']['n_socket']))
        print("n_core     : {}".format(json['nfvi']['n_core']))
        print("message    : DON'T STAY UP ALL NIGHT")

    def _get(self,uri):
        path =  'http://' + self._host + ':' + str(self._port) + uri
        response = requests.get(path)
        r = response.text
        json_dict = json.loads(r)
        if (json_dict['result']['success']):
            return json_dict
        else:
            raise Exception("miss");

    def _delete(self, uri):
        path =  'http://' + self._host + ':' + str(self._port) + uri
        try:
            response = requests.delete(path)
            r = response.text
            json_dict = json.loads(r)
            return json_dict
        except Exception as ex:
            print("error: {}".format(str(ex)))
            print("content: {}".format(response.content))
            exit(-1)

    def _put(self, uri, data):
        path =  'http://' + self._host + ':' + str(self._port) + uri
        try:
            response = requests.put(path, data)
            r = response.text
            json_dict = json.loads(r)
            return json_dict
        except Exception as ex:
            print("error: {}".format(str(ex)))
            print("content: {}".format(response.content))
            exit(-1)

    def _post(self, uri, data):
        path =  'http://' + self._host + ':' + str(self._port) + uri
        try:
            response = requests.post(path, data)
            r = response.text
            json_dict = json.loads(r)
            return json_dict
        except Exception as ex:
            print("error: {}".format(str(ex)))
            print("content: {}".format(response.content))
            exit(-1)

    def __init__(self, host='localhost', port=8888):
        self._host = host
        self._port = port
        res = self._get('/')
        self.sync()

    def sync(self):
        res = self._get('/system')
        if (res['result']['success']):
            self._this = res['nfvi']
        else:
            raise Exception('Miss: {}'.format(msg))

    def ping(self):
        start = time.time()
        res = self._get('/')
        delta = time.time() - start
        return delta

    def host(self): return self._host
    def port(self): return self._port

    def list_vnfs(self):
        res = self._get('/vnfs')
        n_vnf = res['n_vnf']
        vnfs = []
        for i in range(n_vnf):
            n = res[str(i)]['name']
            vnf0 = vnf.Vnf(n, self)
            vnfs.append(vnf0)
        return vnfs

    def get_vnf(self, name):
        vnfs = self.list_vnfs()
        for v in vnfs:
            if (name == v.name()):
                return v
        return None

    def alloc_vnf(self, iname, cname):
        uri = '/vnfs/' + iname
        data  = '{\n'
        data += '  \"cname\" : \"{}\"\n'.format(cname)
        data += '}\n'
        res = self._post(uri, data)
        if (res['result']['success'] == False):
            msg = res['result']['msg']
            raise Exception('Miss: {}'.format(msg))
        return self.get_vnf(iname)

    def delete_vnf(self, iname):
        uri = '/vnfs/' + iname
        res = self._delete(uri)
        if (res['result']['success'] == False):
            msg = res['result']['msg']
            raise Exception('Miss: {}'.format(msg))

    def list_ports(self):
        res = self._get('/ports')
        n_port = res['n_port']
        ports = []
        for i in range(n_port):
            name = res[str(i)]['name']
            port0 = port.Port(name, self)
            ports.append(port0)
        return ports

    def get_port(self, name):
        ports = self.list_ports()
        for p in ports:
            if (name == p.name()):
                return p
        return None

    def alloc_port(self, iname, data):
        uri = '/ports/' + iname
        res = self._post(uri, data)
        if (res['result']['success'] == False):
            msg = res['result']['msg']
            raise Exception('Miss: {}'.format(msg))
        return self.get_port(iname)

    def alloc_port_pci(self, iname, pciaddr):
        data  = '{\n'
        data += '  \"cname\" : \"pci\",\n'
        data += '  \"options\" : {\n'
        data += '     \"pciaddr\" : \"{}\"\n'.format(pciaddr)
        data += '  }\n'
        data += '}\n'
        return self.alloc_port(iname, data)

    def alloc_port_tap(self, iname, ifname):
        data  = '{\n'
        data += '  \"cname\" : \"tap\",\n'
        data += '  \"options\" : {\n'
        data += '     \"ifname\" : \"{}\"\n'.format(ifname)
        data += '  }\n'
        data += '}\n'
        return self.alloc_port(iname, data)

    def alloc_port_virt(self, iname):
        data  = '{\n'
        data += '  \"cname\" : \"virt\"\n'
        data += '}\n'
        return self.alloc_port(iname, data)

    def delete_port(self, iname):
        uri = '/ports/' + iname
        res = self._delete(uri)
        if (res['result']['success'] == False):
            msg = res['result']['msg']
            raise Exception('Miss: {}'.format(msg))

    def list_ppps(self):
        res = self._get('/ppps')
        n_ele = res['n_ele']
        ppps = []
        for i in range(n_ele):
            name = res[str(i)]['name']
            ppp0 = ppp.PPP(name, self)
            ppps.append(ppp0)
        return ppps

    def get_ppp(self, name):
        ppps = self.list_ppps()
        for p in ppps:
            if (name == p.name()):
                return p
        return None

    def alloc_ppp(self, name, left, right):
        r_name = right.name()
        l_name = left.name()
        uri = '/ppps/' + name
        data  = '{\n'
        data += '  \"left\" : \"{}\",\n'.format(l_name)
        data += '  \"right\" : \"{}\"\n'.format(r_name)
        data += '}\n'
        res = self._post(uri, data)
        if (res['result']['success'] == False):
            msg = res['result']['msg']
            raise Exception('Miss: {}'.format(msg))
        return self.get_ppp(name)

    def delete_ppp(self, name):
        uri = '/ppps/' + name
        res = self._delete(uri)
        if (res['result']['success'] == False):
            msg = res['result']['msg']
            raise Exception('Miss: {}'.format(msg))

    def _get_pcidev_details(self, dev_id):
        '''This function gets additional details for a PCI device'''
        device = {}
        extra_info = _check_output(["lspci", "-vmmks", dev_id]).splitlines()

        # parse lspci details
        for line in extra_info:
            if len(line) == 0:
                continue
            name, value = line.decode().split("\t", 1)
            name = name.strip(":") + "_str"
            device[name] = value
        # check for a unix interface name
        sys_path = "/sys/bus/pci/devices/%s/net/" % dev_id
        if exists(sys_path):
            device["Interface"] = ",".join(os.listdir(sys_path))
        else:
            device["Interface"] = ""
        # check if a port is used for ssh connection
        device["Ssh_if"] = False
        device["Active"] = ""
        return device

    def get_nic_details(self):
        devices = {}
        dev = {};
        dev_lines = _check_output(["lspci", "-Dvmmn"]).splitlines()
        for dev_line in dev_lines:
            if (len(dev_line) == 0):
                if dev["Class"] == self.ETHERNET_CLASS:
                    #convert device and vendor ids to numbers, then add to global
                    dev["Vendor"] = int(dev["Vendor"],16)
                    dev["Device"] = int(dev["Device"],16)
                    devices[dev["Slot"]] = dict(dev) # use dict to make copy of dev
            else:
                name, value = dev_line.decode().split("\t", 1)
                dev[name.rstrip(":")] = value

        # based on the basic info, get extended text details
        for d in devices.keys():
            # get additional info and add it to existing data
            devices[d] = devices[d].copy()
            update_dict = self._get_pcidev_details(d).items()
            devices[d].update(update_dict)

            # add igb_uio to list of supporting modules if needed
            if "Module_str" in devices[d]:
                for driver in self.dpdk_drivers:
                    if driver not in devices[d]["Module_str"]:
                        devices[d]["Module_str"] = devices[d]["Module_str"] + ",%s" % driver
            else:
                devices[d]["Module_str"] = ",".join(self.dpdk_drivers)

            # make sure the driver and module strings do not have any duplicatesj

            if ("Driver_str" in devices[d]):
                modules = devices[d]["Module_str"].split(",")
                if devices[d]["Driver_str"] in modules:
                    modules.remove(devices[d]["Driver_str"])
                    devices[d]["Module_str"] = ",".join(modules)
        return devices

