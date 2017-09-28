
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <slankdev/string.h>
#include <slankdev/exception.h>
#include <dpdk/dpdk.h>

#include <ssn_port.h>
#include <ssn_common.h>
#include <ssn_log.h>
#include <ssn_vnf.h>
#include <ssn_vnf_ext.h>
#include <ssn_port.h>
#include <ssn_port_stat.h>
#include <ssn_ma_port.h>
#include <ssn_log.h>
#include <ssn_cpu.h>
#include <ssn_common.h>
#include <ssn_thread.h>

#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <vector>
#include <assert.h>
#include <slankdev/string.h>
#include <slankdev/exception.h>


class ssn_vnf_impl {
  /* going to use array */
  std::vector<ssn_vnf_port*> ports; /*! VNF's port vector                  */
  std::vector<uint32_t> tids;       /*! thread-ids for running-information */
  std::vector<size_t> lcores;       /*! lcore-ids assined                  */
  uint32_t coremask;                /*! running coremask                   */
 public:
  virtual void deploy_imple(void*) = 0;
  virtual void undeploy_imple() = 0;
  virtual bool is_running() const = 0;
  ssn_vnf_impl(size_t np) : n_ports(np) {}
};

class ssn_vnf_ext {
 private:
  static void _vnf_ext_thread_spawner(void* vnf_impl_instance_)
  {
    ssn_vnf_impl* vnf_impl = reinterpret_cast<ssn_vnf_impl*>(vnf_impl_instance_);
    vnf_impl->deploy_imple(nullptr);
  }
  std::vector<ssn_vnf_impl*> impls; /*! Thread implementations             */
  std::vector<size_t> lcores;       /*! lcore-ids assined                  */
  uint32_t coremask;                /*! running coremask                   */

 protected:
  void add_impl(ssn_vnf_impl* impl) { impls.push_back(impl); }
  size_t tx_burst(size_t pid, size_t aid, rte_mbuf** mbufs, size_t n_mbufs)
  { return ports[pid]->tx_burst(aid, mbufs, n_mbufs); }
  size_t rx_burst(size_t pid, size_t aid, rte_mbuf** mbufs, size_t n_mbufs)
  { return ports[pid]->rx_burst(aid, mbufs, n_mbufs); }

 public:
  ssn_vnf_ext(size_t n_ports_a) : ports(n_ports_a) {}
  virtual ~ssn_vnf_ext() {}
  size_t n_ports() const { return ports.size(); }
  void attach_port(size_t pid, ssn_vnf_port* p) { ports.at(pid) = p; }

  void deploy(uint32_t lcore_mask)
  {
    this->coremask = lcore_mask;

    lcores.clear();
    for (size_t i=0; i<32; i++) {
      if ((lcore_mask & (0x1<<i)) != 0)
        lcores.push_back(i);
    }
    tids.resize(lcores.size());

    for (size_t i=0; i<ports.size(); i++) {
      ports[i]->configure(lcores.size(), lcores.size());
    }

    for (size_t i=0; i<lcores.size(); i++) {
      tids[i] = ssn_thread_launch(_vnf_ext_thread_spawner, this, lcores[i]);
    }
  }

  void undeploy()
  {
    assert(tids.size() == lcores.size());

    undeploy_imple();
    for (size_t i=0; i<lcores.size(); i++) {
      ssn_thread_join(tids[i]);
    }
    printf("undeploy succes\n");
  }

  size_t get_aid_from_lcoreid(size_t lcore_id) const
  {
    size_t n_lcores = lcores.size();
    for (size_t i=0; i<n_lcores; i++) {
      if (lcores[i] == lcore_id) return i;
    }
    throw slankdev::exception(
        "vnf::get_aid_from_lcoreid: not found aid (lcoreid is invalid?)");
  }

  size_t get_aid() const
  {
    size_t lcore_id = ssn_lcore_id();
    return get_aid_from_lcoreid(lcore_id);
  }

  void debug_dump(FILE* fp) const
  {
    fprintf(fp, " state   : %s \r\n", this->is_running()?"running":"stop");
    fprintf(fp, " coremask: ");
    if (is_running()) {
      fprintf(fp, "0x%04x ", coremask);
      fprintf(fp, "%s \r\n", slankdev::ntob<8>(coremask).c_str());
      auto n = tids.size();
      for (size_t i=0; i<n; i++) {
        fprintf(fp, "    tid[%zd] 0x%08x \r\n", i, tids[i]);
      }
    } else {
      fprintf(fp, "-\r\n");
    }
    fprintf(fp, " n_ports : %zd \r\n", this->n_ports());
  }
};

class vnf_test_impl_port0 : public ssn_vnf_impl {
 public:
  virtual void deploy_imple(void* acc_id_) override {}
  virtual void undeploy_imple() override {}
  virtual bool is_running() const override { return false; }
};

class vnf_test_impl_port1 : public ssn_vnf_impl {
 public:
  virtual void deploy_imple(void* acc_id_) override {}
  virtual void undeploy_imple() override {}
  virtual bool is_running() const override { return false; }
};

class vnf_test : public ssn_vnf_ext {
 public:
  vnf_test()
  {
    add_impl(new vnf_test_impl_port0(np));
    add_impl(new vnf_test_impl_port1(np));
  }
};


char waitmsg(const char* msg)
{
  printf(msg);
  return getchar();
}

void VNF_DUMP(ssn_vnf_ext* vnf)
{
  printf("\n");
  printf("vnfptr: %p \r\n", vnf);
  vnf->debug_dump(stdout);
  printf("\n");
}

int main(int argc, char** argv)
{
  ssn_init(argc, argv);
  size_t n_ports = ssn_dev_count();
  if (n_ports != 2) {
    std::string err = slankdev::format(
        "n_ports is not 2 (current %zd)",
        ssn_dev_count());
    throw slankdev::exception(err.c_str());
  }

  ssn_vnf_port* port0 = new ssn_vnf_port(0, 2, 4); // dpdk0
  ssn_vnf_port* port1 = new ssn_vnf_port(1, 2, 4); // dpdk1
  printf("\n");
  port0->debug_dump(stdout); printf("\n");
  port1->debug_dump(stdout); printf("\n");

  vnf_test* vnf0 = new vnf_test;
  vnf0->attach_port(0, port0); // attach dpdk0 to vnf0
  vnf0->attach_port(1, port1); // attach dpdk1 to vnf0
  VNF_DUMP(vnf0);

  /* run with 2 cores */
  waitmsg("press [enter] to deploy with 2 lcores...\n");
  vnf0->undeploy();
  vnf0->deploy(0x0c);
  VNF_DUMP(vnf0);

  /* run with 4 cores */
  waitmsg("press [enter] to deploy with 4 lcores...\n");
  vnf0->undeploy();
  vnf0->deploy(0x3c);
  VNF_DUMP(vnf0);

  /* undeploy */
  waitmsg("press [enter] to undeploy ");
  vnf0->undeploy();
  VNF_DUMP(vnf0);

fin:
  delete vnf0;
  delete port0;
  delete port1;
  ssn_fin();
}


