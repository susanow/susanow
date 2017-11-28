
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

#include <ssn_nfvi.h>

ssn_nfvi* _nfvip = nullptr;

static void signal_handler(int signum)
{
  if (signum == SIGINT || signum == SIGTERM) {
    if (_nfvip) {
      printf("recv signal(%d), then exit...\n", signum);
      _nfvip->stop();
    }
  }
}

void ssn_nfvi::run(uint16_t rest_server_port)
{
  /*
   * Launch REST API server
   */
  std::thread rest_api(rest_api_thread, this, &app, rest_server_port);

  /*
   * Running loop
   */
  running = true;
  while (running) {
    slankdev::signal(SIGINT, signal_handler);
    slankdev::signal(SIGTERM, signal_handler);
    sleep(1);
  }

  /*
   * Undeploy all vnfs
   */
  const size_t n_vnf = vnfs.size();
  for (size_t i=0; i<n_vnf; i++) {
    if (vnfs[i]->is_running())
      vnfs[i]->undeploy();
  }

  rest_api.join();
}


void ssn_nfvi::debug_dump(FILE* fp) const
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

  printf("[+] port-pathc-panels (n:name)\n");
  const size_t n_ppps = ppps.size();
  for (size_t i=0; i<n_ppps; i++) {
    printf("ppps[%zd] n=%s \n", i, ppps[i]->name.c_str());
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


ssn_nfvi::ssn_nfvi(int argc, char** argv, ssn_log_level ll)
  : mp(nullptr), timer_sched(nullptr), running(false)
{
  ssn_log_set_level(ll);
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

  _nfvip = this;
}

ssn_nfvi::~ssn_nfvi()
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

  /*
   * Free ppp objects
   */
  size_t n_ppp = ppps.size();
  for (size_t i=0; i<n_ppp; i++) {
    delete ppps[i];
  }

  rte_mempool_free(mp);
  ssn_fin();
}


