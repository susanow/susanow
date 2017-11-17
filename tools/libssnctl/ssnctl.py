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

import json
import time
import sys
import pprint
import requests
HOST='10.0.0.3:8888'

def get_json_dict_from_url(path):
    try:
        response = requests.get(path)
        r = response.text
        json_dict = json.loads(r)
        return json_dict
    except Exception as ex:
        print("error: {}".format(str(ex)))
        print("content: {}".format(response.content))
        exit(-1)

def put_json_dict_to_url(path, data):
    try:
        response = requests.put(path, data)
        r = response.text
        json_dict = json.loads(r)
        return json_dict
    except Exception as ex:
        print("error: {}".format(str(ex)))
        print("content: {}".format(response.content))
        exit(-1)

class vnf:
    name = '-1vnfname'

    def __init__(self, name):
        self.name = name

    def get_perfred(self):
        URL = 'http://' + HOST + '/vnfs/' + self.name
        json_dict = get_json_dict_from_url(URL)
        if (json_dict["result"]["success"]):
            vnf = json_dict["vnf"]
            return vnf['perfred']
        else:
            print("miss: {}".format(json_dict["result"]["msg"]))
            exit(-1)

    def get_rx_rate(self):
        URL = 'http://' + HOST + '/vnfs/' + self.name
        json_dict = get_json_dict_from_url(URL)
        if (json_dict["result"]["success"]):
            vnf = json_dict["vnf"]
            return vnf['rxrate']
        else:
            print("miss: {}".format(json_dict["result"]["msg"]))
            exit(-1)


    def get_coremask(self):
        URL = 'http://' + HOST + '/vnfs/' + self.name
        json_dict = get_json_dict_from_url(URL)
        if (json_dict["result"]["success"]):
            vnf = json_dict["vnf"]
            return vnf['coremask']
        else:
            print("miss: {}".format(json_dict["result"]["msg"]))
            exit(-1)

    def set_coremask(self, coremask):
        bid = 0
        URL = 'http://' + HOST + '/vnfs/' + self.name + '/coremask/' + str(bid)
        data = "{\"coremask\": " + str(coremask) + "}"
        result = put_json_dict_to_url(URL, data)["result"]
        if (result["success"]): return
        else: print("miss: {}".format(result["msg"]))

    def is_running(self):
        URL = 'http://' + HOST + '/vnfs/' + self.name
        json_dict = get_json_dict_from_url(URL)
        if (json_dict["result"]["success"]):
            vnf = json_dict["vnf"]
            return vnf['running']
        else:
            print("miss: {}".format(json_dict["result"]["msg"]))
            exit(-1)

    def deploy(self):
        URL = 'http://' + HOST + '/vnfs/' + self.name + '/deploy'
        result = put_json_dict_to_url(URL, "")["result"]
        if (result["success"]): return
        else: print("miss: {}".format(result["msg"]))

    def undeploy(self):
        URL = 'http://' + HOST + '/vnfs/' + self.name + '/undeploy'
        result = put_json_dict_to_url(URL, "")["result"]
        if (result["success"]): return
        else: print("miss: {}".format(result["msg"]))

    def reset(self):
        URL = 'http://' + HOST + '/vnfs/' + self.name + '/reset'
        result = put_json_dict_to_url(URL, "")["result"]
        if (result["success"]): return
        else: print("miss: {}".format(result["msg"]))

    def summary(self):
        pr = self.get_perfred()
        cm = self.get_coremask()
        r  = self.is_running()
        print('name    : {}'.format(self.name))
        print('perfred : {}'.format(pr))
        # print('coremask: {}'.format(bin(cm)))
        print('coremask: {}'.format(cm))
        print('running : {}'.format(r))

    def d2op_out(self):
        cm = self.get_coremask()
        acm = next_coremask(cm)
        if (acm == -1):
            print('no such computing resource... :(')
            return
        self.undeploy()
        self.reset()
        self.set_coremask(acm)
        self.deploy()

    def d2op_in(self):
        return

    def get_block_width():
        cm = self.get_coremask()
        return popcnt(cm)

def popcnt(num):
    return bin(num).count('1')

def next_coremask(coremask):
    array = [4, 12, 60]
    for i in range(len(array) - 1):
        if (array[i] == coremask):
            return array[i+1]
    return -1


def get_vnf(name):
    return vnf(name)


