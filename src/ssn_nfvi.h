
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

#include <stdio.h>
#include <stdint.h>
#include <stddef.h>

#include <ssn_log.h>
#include <ssn_port.h>
#include <ssn_common.h>
#include <ssn_timer.h>
#include <ssn_vnf_catalog.h>
#include <ssn_port_catalog.h>


class ssn_nfvi final {
 private:

  static void _timercallback(void* arg)
  {
    ssn_nfvi* nfvi = reinterpret_cast<ssn_nfvi*>(arg);
    ssn_port_stat_update(nullptr);

    size_t n_vnf = nfvi->vnfs.size();
    for (size_t i=0; i<n_vnf; i++) {
      auto* vnf = nfvi->vnfs[i];
      vnf->update_stats();
    }
  }

  rte_mempool* mp;
  ssn_vnf_catalog  vnf_catalog;
  ssn_port_catalog port_catalog;

  ssn_timer_sched* timer_sched;
  uint64_t timer_thread_tid;
  const size_t lcoreid_timer  = 1;
  std::vector<ssn_timer*> tims;

  std::vector<ssn_vnf*>      vnfs;
  std::vector<ssn_vnf_port*> ports;

 public:

  void del_port(ssn_vnf_port* port)
  {
    size_t n_ele = ports.size();
    for (size_t i=0; i<n_ele; i++) {
      if (ports[i] == port) {
        ports.erase(ports.begin() + i);
        return ;
      }
    }
    std::string err = "ssn_nfvi::del_port: ";
    err += slankdev::format("not found port (%s)", port->name);
    throw slankdev::exception(err.c_str());
  }

  void del_vnf(ssn_vnf* vnf)
  {
    size_t n_ele = vnfs.size();
    for (size_t i=0; i<n_ele; i++) {
      if (vnfs[i] == vnf) {
        vnfs.erase(vnfs.begin() + i);
        return ;
      }
    }
    std::string err = "ssn_nfvi::del_vnf: ";
    err += slankdev::format("not found vnf (%s)", vnf->name);
    throw slankdev::exception(err.c_str());
  }

  const std::vector<ssn_vnf*>& get_vnfs() const { return vnfs; }
  const std::vector<ssn_vnf_port*>& get_ports() const { return ports; }
  const ssn_vnf_catalog& get_vcat() const { return vnf_catalog; }
  const ssn_port_catalog& get_pcat() const { return port_catalog; }

  void vnf_alloc_from_catalog(const char* catname, const char* instancename)
  {
    ssn_vnf* vnf = vnf_catalog.alloc_vnf(
        catname, instancename);
    vnfs.push_back(vnf);
  }

  void port_alloc_from_catalog(const char* catname,
      const char* instancename, void* arg)
  {
    ssn_vnf_port* port = port_catalog.alloc_port(
        catname, instancename, arg);
    ports.push_back(port);
  }

  void vnf_register_to_catalog(const char* catname, ssn_vnfallocfunc_t f)
  { vnf_catalog.register_vnf(catname, f); }

  void port_register_to_catalog(const char* catname, ssn_portallocfunc_t f)
  { port_catalog.register_port(catname , f); }

  ssn_nfvi(int argc, char** argv) : mp(nullptr), timer_sched(nullptr)
  {
    ssn_init(argc, argv);
    const size_t n_ports = ssn_dev_count();
    for (size_t i=0; i<n_ports; i++) {
      dpdk::eth_dev_detach(i);
    }
    mp = dpdk::mp_alloc("NFVi");

    timer_sched = new ssn_timer_sched(lcoreid_timer);
    timer_thread_tid = ssn_native_thread_launch(
        ssn_timer_sched_poll_thread, timer_sched, lcoreid_timer);

    uint64_t one_sec = ssn_timer_get_hz();
    add_timer(new ssn_timer(_timercallback, this, one_sec));
    printf("FINISH nfvi initialization\n\n");
  }

  virtual ~ssn_nfvi()
  {
    /*
     * Free Objects about Timer
     */
    size_t n_tims = tims.size();
    for (size_t i=0; i<n_tims; i++) {
      ssn_timer* tim = tims[i];
      del_timer(tim);
    }
    delete timer_sched;

    /*
     * Free VNF Objects
     */
    size_t n_vnf = vnfs.size();
    for (size_t i=0; i<n_vnf; i++) {
      delete vnfs[i];
    }

    /*
     * Free Port Objects
     */
    size_t n_port = ports.size();
    for (size_t i=0; i<n_port; i++) {
      delete ports[i];
    }

    rte_mempool_free(mp);
    ssn_fin();
  }

  void add_timer(ssn_timer* tim)
  {
    tims.push_back(tim);
    timer_sched->add(tim);
  }

  void del_timer(ssn_timer* tim)
  {
    size_t n_tims = tims.size();
    for (size_t i=0; i<n_tims; i++) {
      if (tims[i] == tim) {
        tims.erase(tims.begin() + i);
        timer_sched->del(tim);
        delete tim;
        return ;
      }
    }
    std::string err;
    err = slankdev::format("ssn_nfvi::del_timer: not found timer %p", tim);
    throw slankdev::exception(err.c_str());
  }

  ssn_vnf_port* find_port(const char* name)
  {
    const size_t n_port = ports.size();
    for (size_t i=0; i<n_port; i++) {
      if (ports[i]->name == name) {
        return ports[i];
      }
    }
    std::string err = "ssn_nfvi::find_port: not found";
    err += slankdev::format("(%s)", name);
    throw slankdev::exception(err.c_str());
  }

  ssn_vnf* find_vnf(const char* name)
  {
    const size_t n_vnf = vnfs.size();
    for (size_t i=0; i<n_vnf; i++) {
      if (vnfs[i]->name == name) {
        return vnfs[i];
      }
    }
    std::string err = "ssn_nfvi::find_vnf: not found";
    err += slankdev::format("(%s)", name);
    throw slankdev::exception(err.c_str());
  }

  void undeploy_all_vnfs()
  {
    const size_t n_vnf = vnfs.size();
    for (size_t i=0; i<n_vnf; i++) {
      vnfs[i]->undeploy();
    }
  }

  void debug_dump(FILE* fp) const
  {
    using std::string;

    printf("\n");

    printf("[+] vnfs (n:name, b:n_block, p:n_port, r:is_running)\n");
    const size_t n_vnfs = vnfs.size();
    for (size_t i=0; i<n_vnfs; i++) {
      const auto* vnf = vnfs[i];
      string name = vnf->name.c_str();
      size_t n_ports  = vnf->n_ports();
      size_t n_blocks = vnf->n_blocks();
      string run = vnf->is_running()?"true":"false";
      printf("vnfs[%zd]: n=%s b=%zd p=%zd r=%s \n",
          i, name.c_str(), n_blocks, n_ports, run.c_str());
    }

    printf("\n");

    printf("[+] ports (n:name)\n");
    const size_t n_ports = ports.size();
    for (size_t i=0; i<n_ports; i++) {
      printf("ports[%zd] n=%s \n", i, ports[i]->name.c_str());
    }

    printf("\n");

    printf("[+] vnf catalog (n:name)\n");
    const size_t n_vcat = vnf_catalog.size();
    for (size_t i=0; i<n_vcat; i++) {
      printf("vcat[%zd] n=%s \n", i, vnf_catalog[i].name.c_str());
    }

    printf("\n");

    printf("[+] port catalog (n:name)\n");
    const size_t n_pcat = port_catalog.size();
    for (size_t i=0; i<n_pcat; i++) {
      printf("pcat[%zd] n=%s \n", i, port_catalog[i].name.c_str());
    }
  }

  struct rte_mempool* get_mp() { return mp; }

}; /* class ssn_nfvi */



