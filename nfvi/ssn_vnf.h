
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
#include <slankdev/vector.h>
#include <ssn_vnf_port.h>
#include <ssn_vnf_vcore.h>
#include <ssn_vnf_block.h>


/**
 * @brief Provide VNF Interface.
 */
class ssn_vnf {
 private:

  /**
   * @brief Configuration port accessors
   * @details
   *   This function must be called after calling ssn_vnf::set_coremask()
   */
  void configre_acc();

 protected:

  slankdev::fixed_size_vector<ssn_vnf_port*> ports;
  std::vector<ssn_vnf_block*> blocks;

 public:
  const std::string name;

  /**
   * @brief get number of ports
   * @return n_ports
   */
  size_t n_ports() const;

  /**
   * @brief get number of blocks
   * @return n_blocks
   */
  size_t n_blocks() const;

  /**
   * @brief constructor
   * @param [in] nport number of ports included vnf.
   */
  ssn_vnf(size_t nport, const char* n);

  /**
   * @brief Check Deployable?
   * @return true deployable
   * @return false not deployable
   */
  bool deployable() const;

  /**
   * @brief reset vnf's running config.
   * @return 0 success
   * @return -1 unsuccess, maybe port is not attached
   * @details
   *   Calling this functions needs that vnf already attached all ports.
   *   This function doesn't detach ports.
   *   This function must be called before colling set_coremask().
   */
  int reset();

  /**
   * @brief set logical coremask to Block
   * @param [in] block_id block id
   * @param [in] cmask coremask
   * @return 0 success
   * @return -1 unsuccess, maybe port is not attached
   * @details
   *   Calling this functions needs that vnf already attached all ports.
   *   ex. If user want bind 2 lcores (lcore2 and lcore4),
   *   coremask is 0x14 (0b00010100)
   *   ex. lcore3 only: coremask is 0x08 (0x00001000)
   *   ex. lcore2 and lcore3: coremask is 0x0c (0x00001100)
   *   User must call this->reset() before calling this function.
   */
  int set_coremask(size_t block_id, uint32_t cmask);

  /**
   * @brief return VNF's all coremask
   * @return coremask
   * @details
   *    if n_blocks:2, block0:0x2, block1:0x1,
   *    this function ret 0x3
   */
  uint32_t get_coremask() const;

  /**
   * @brief return VNF's all coremask
   * @param [in] bid block id
   * @return coremask
   * @details
   *    if n_blocks:2, block0:0x2, block1:0x1,
   *    this function(0) ret 0x2
   *    this function(1) ret 0x1
   */
  uint32_t get_coremask(size_t bid) const;

  /**
   * @brief check vnf is deletable
   * @return true deletable
   * @return false undeletable
   */
  bool is_deletable() const;

  /**
   * @brief get vnf's port as pointer to get stats
   * @param [in] pid port id
   * @return ssn_vnf_port pointer
   */
  const ssn_vnf_port* get_port(size_t pid) const;

  /**
   * @brief get vnf's block as pointer to get stats
   * @param [in] bid block id
   * @return ssn_vnf_block pointer
   */
  const ssn_vnf_block* get_block(size_t bid) const;

  /**
   * @brief Debug output
   * @param fp FILE* file pointer to output
   */
  void debug_dump(FILE* fp) const;

  /**
   * @brief Deploy VNF
   * @return 0 success
   * @return -1 unsuccess, some-error occured
   * @details
   *   Calling this functions needs that vnf already attached all ports.
   *   This function calles ssn_vnf::configure_acc() inner itselfs.
   */
  int deploy();

  /**
   * @brief Deploy VNF
   */
  void undeploy();

  /**
   * @brief update vnf's status info
   * @details
   *   this function must be call per 1 second
   *   by Hypervisor.
   */
  void update_stats();

  /**
   * @brief check vnf is running
   * @return if vnf is running, retval is true. else retval is false
   */
  bool is_running() const;

  /**
   * @brief attach vnf's port
   * @param [in] pid vnf's port id
   * @param [in] port port class's pointer to attach to vnf
   * @return 0 success
   * @return -1 invalid port or already attached
   */
  int attach_port(size_t pid, ssn_vnf_port* port);

  /**
   * @brief dettach vnf's port
   * @param [in] pid vnf's port id
   * @return 0 success
   * @return -1 port does not attached
   */
  int dettach_port(size_t pid);

  /**
   * @brief get performance reduction from attached ports
   * @return perfred 0.8 means 20% perf-reduction.
   */
  double get_perf_reduction() const;

  /**
   * @brief get rx performance from attached ports
   * @return rx performance [Mbps]
   */
  size_t get_rx_rate() const;

}; /* class ssn_vnf */




