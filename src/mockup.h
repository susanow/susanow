

/*
 * MIT License
 *
 * Copyright (c) 2017 Susanow
 * Copyright (c) 2017 Hiroki SHIROKURA
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#pragma once


void read_from_file(ssn_vnf_catalog& cat, const char* filename)
{
  auto obj = ldopen(filename);
  auto name = obj.getname();
  auto allocator = obj.getfunc();
  cat.register_vnf(name, allocator);
}

void usage()
{
  ssn_vnf_catalog cat;
  cat.register_vnf("l2fwd1b", l2fwd1b_allocator);
  cat.register_vnf("l2fwd2b", l2fwd2b_allocator);
  read_from_file(cat, "newvnf.o");
}

void usage1()
{
  rte_mempool* mp = get_mp();
  ssn_vnf_port* port = alloc_port_from_catalog("pci", "pci0");
  port->configure_hw(4,4,mp);
}

void usage2()
{
  ssn_vnf* vnf = alloc_vnf_from_catalog("l2fwd1b", "vnf0");
  append_vnf(vnf);
  vnf->attach_port(0, port0);
  vnf->attach_port(1, port1);
  vnf->reset_allport_acc();
  vnf->set_coremask(0, 0b00000010);
  vnf->deploy();
  vnf->undeploy();
}



