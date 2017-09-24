
API {#index}
===

<!--
  BSD LICENSE

  Copyright 2013-2017 6WIND S.A.

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions
  are met:

    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in
      the documentation and/or other materials provided with the
      distribution.
    * Neither the name of 6WIND S.A. nor the names of its
      contributors may be used to endorse or promote products derived
      from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
-->

There are many libraries, so their headers may be grouped by topics:

- **device**:
  [dev]                (@ref rte_dev.h),
  [ethdev]             (@ref rte_ethdev.h),
  [ethctrl]            (@ref rte_eth_ctrl.h),
  [rte_flow]           (@ref rte_flow.h),
  [rte_flow_driver]    (@ref rte_flow_driver.h),
  [cryptodev]          (@ref rte_cryptodev.h),
  [eventdev]           (@ref rte_eventdev.h),
  [devargs]            (@ref rte_devargs.h),
  [PCI]                (@ref rte_pci.h)

- **device specific**:
  [bond]               (@ref rte_eth_bond.h),
  [vhost]              (@ref rte_vhost.h),
  [KNI]                (@ref rte_kni.h),
  [ixgbe]              (@ref rte_pmd_ixgbe.h),
  [i40e]               (@ref rte_pmd_i40e.h),
  [crypto_scheduler]   (@ref rte_cryptodev_scheduler.h)

- **memory**:
  [memseg]             (@ref rte_memory.h),
  [memzone]            (@ref rte_memzone.h),
  [mempool]            (@ref rte_mempool.h),
  [malloc]             (@ref rte_malloc.h),
  [memcpy]             (@ref rte_memcpy.h)

- **timers**:
  [cycles]             (@ref rte_cycles.h),
  [timer]              (@ref rte_timer.h),
  [alarm]              (@ref rte_alarm.h)

- **locks**:
  [atomic]             (@ref rte_atomic.h),
  [rwlock]             (@ref rte_rwlock.h),
  [spinlock]           (@ref rte_spinlock.h)

