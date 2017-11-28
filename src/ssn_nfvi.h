
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
#include <unistd.h>
#include <mutex>
#include <crow.h>

#include <ssn_log.h>
#include <ssn_port.h>
#include <ssn_common.h>
#include <ssn_timer.h>
#include <ssn_vnf_catalog.h>
#include <ssn_port_catalog.h>
#include <ssn_rest_api.h>
#include <slankdev/exception.h>
#include <slankdev/signal.h>


class ssn_nfvi final {
 private:

  rte_mempool* mp;
  ssn_vnf_catalog  vnf_catalog;
  ssn_port_catalog port_catalog;

  ssn_timer_sched* timer_sched;
  uint64_t timer_thread_tid;
  const size_t lcoreid_timer  = 1;
  std::vector<ssn_timer*> tims;

  std::vector<ssn_vnf*>      vnfs;
  std::vector<ssn_vnf_port*> ports;
  std::vector<ssn_vnf_port_patch_panel*> ppps;
  bool running;
  crow::App<Middleware> app;

 private:

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

  static void _timercallback(void* arg)
  {
    ssn_nfvi* nfvi = reinterpret_cast<ssn_nfvi*>(arg);
    ssn_port_stat_update(nullptr);

    size_t n_vnf = nfvi->vnfs.size();
    for (size_t i=0; i<n_vnf; i++) {
      auto* vnf = nfvi->vnfs[i];
      if (vnf->is_running()) vnf->update_stats();
    }
  }
 public:

  ssn_nfvi(int argc, char** argv, ssn_log_level ll=SSN_LOG_INFO);
  virtual ~ssn_nfvi();
  void run(uint16_t rest_server_port);
  void stop();
  void debug_dump(FILE* fp) const;
  struct rte_mempool* get_mp() { return mp; }

  const std::vector<ssn_vnf*>& get_vnfs() const { return vnfs; }
  const std::vector<ssn_vnf_port*>& get_ports() const { return ports; }
  const ssn_vnf_catalog& get_vcat() const { return vnf_catalog; }
  const ssn_port_catalog& get_pcat() const { return port_catalog; }
  const std::vector<ssn_vnf_port_patch_panel*>& get_ppps() const { return ppps; }

  void del_port(ssn_vnf_port* port)
  {
    size_t n_ele = ports.size();
    for (size_t i=0; i<n_ele; i++) {
      if (ports[i] == port) {
        ports.erase(ports.begin() + i);
        delete port;
        return ;
      }
    }
    std::string err = "ssn_nfvi::del_port: ";
    err += slankdev::format("not found port (%s)", port->name.c_str());
    throw slankdev::exception(err.c_str());
  }

  void del_vnf(ssn_vnf* vnf)
  {
    size_t n_ele = vnfs.size();
    for (size_t i=0; i<n_ele; i++) {
      if (vnfs[i] == vnf) {
        vnfs.erase(vnfs.begin() + i);
        delete vnf;
        return ;
      }
    }
    std::string err = "ssn_nfvi::del_vnf: ";
    err += slankdev::format("not found vnf (%s)", vnf->name.c_str());
    throw slankdev::exception(err.c_str());
  }

  void del_ppp(ssn_vnf_port_patch_panel* ppp)
  {
    size_t n_ele = vnfs.size();
    for (size_t i=0; i<n_ele; i++) {
      if (ppps[i] == ppp) {
        ppps.erase(ppps.begin() + i);
        delete ppp;
        return ;
      }
    }
    std::string err = "ssn_nfvi::del_ppp: ";
    err += slankdev::format("not found ppp (%s)", ppp->name.c_str());
    throw slankdev::exception(err.c_str());
  }

  /**
   * @brief Allocate new VNF from catalog
   * @param [in] cname catalog-name
   * @param [in] iname instance-name
   * @return nullptr iname or cname is invalid
   * @return vnfs pointer
   */
  ssn_vnf* vnf_alloc_from_catalog(const char* cname, const char* iname);

  /**
   * @brief Allocate new Port from catalog
   * @param [in] cname catalog-name
   * @param [in] iname instance-name
   * @return nullptr iname or cname is invalid
   * @return port's pointer
   */
  ssn_vnf_port*
  port_alloc_from_catalog(const char* cname, const char* iname, void* arg);

  /**
   * @brief wrapper function of ssn_nfvi::port_alloc_from_catalog()
   * @param [in] iname instance-name
   * @param [in] ifname linux-if-name
   * @return nullptr iname or cname is invalid
   * @return port's pointer
   */
  ssn_vnf_port* port_alloc_tap(const char* iname, const char* ifname);

  /**
   * @brief wrapper function of ssn_nfvi::port_alloc_from_catalog()
   * @param [in] iname instance-name
   * @param [in] pciaddr pci-address string
   * @return nullptr iname or cname is invalid
   * @return port's pointer
   */
  ssn_vnf_port* port_alloc_pci(const char* iname, const char* pciaddr);

  /**
   * @brief wrapper function of ssn_nfvi::port_alloc_from_catalog()
   * @param [in] iname instance-name
   * @return nullptr iname or cname is invalid
   * @return port's pointer
   */
  ssn_vnf_port* port_alloc_virt(const char* iname);

  /**
   * @brief Allocate new Port-patch-panel
   * @param [in] iname instance-name
   * @param [in] r right-port's pointer
   * @param [in] l left-port's pointer
   * @return nullptr iname or cname is invalid
   * @return ppp's pointer
   */
  ssn_vnf_port_patch_panel*
  ppp_alloc(const char* iname, ssn_vnf_port* r, ssn_vnf_port* l);

  /*
   * @brief register new vnf-catalog-element
   * @param [in] cname catalog-name
   * @param [in] f allocator-function
   * @details
   *    if cname was already registered,
   *    this function throws exception.
   */
  void vnf_register_to_catalog(const char* cname, ssn_vnfallocfunc_t f);

  /*
   * @brief register new port-catalog-element
   * @param [in] cname catalog-name
   * @param [in] f allocator-function
   * @details
   *    if cname was already registered,
   *    this function throws exception.
   */
  void port_register_to_catalog(const char* cname, ssn_portallocfunc_t f);

  /**
   * @brief find port by name
   * @param [in] name port name
   * @return valid-pointer found port's pointer
   * @return nullptr not found port
   */
  ssn_vnf_port* find_port(const char* name);

  /**
   * @brief find vnf by name
   * @param [in] name vnf name
   * @return valid-pointer found vnf's pointer
   * @return nullptr not found vnf
   */
  ssn_vnf* find_vnf(const char* name);

  /**
   * @brief find port-patch-panel by name
   * @param [in] name ppp name
   * @return valid-pointer found ppp's pointer
   * @return nullptr not found ppp
   */
  ssn_vnf_port_patch_panel* find_ppp(const char* name);

}; /* class ssn_nfvi */

