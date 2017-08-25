
Introduction and Motivation
===========================

Motivation
^^^^^^^^^^
Motivation

Using DPDK is the smartest way to implement
High Performance Networking (HPN) software on x86/x64 architecture.
That have very very powerful APIs to operate low layer architecture,
ex. to occupy CPU logical cores and polling each NICs queues.

But I can add more powerfull function to operate manycore machine.
for example, CPU pinning can be more flexible. The best CPU pinning pattern
varies according to circumstance by trafficpattern. There are heavy traffics
comming, It have to assign many locical cores to RX thread.
But else (little traffic comming), it have to assign a little cores to RX thread.
So CPU pinning pattern varies.

So far, operator must reconfig CPU Pinning Pattern from traffic pattern,
but Susanoo can self reconfiguration dynamically from traffic pattern
from RX information.
We call that, DRCP (Dynamic Re- CpuPinning).
It may get better with manycore CPU, xeon phi. and NUMA multi socket platforms.

User to develop with Susanow can use these algorithm easy and extend too.
We try to use C++11 to Implement Susanow with safe and extensible implementation.
If you worse code, tell us and we become very very happy.


Good Point DPDK
^^^^^^^^^^^^^^^
 - very very faster than socket
 - cpu pinning each cores
 - like a small os
 - access all HW


Bad Point DPDK
^^^^^^^^^^^^^^
 - difficult to use memory cache
 - difficult cpu pinning
 - difficult numa
 - use electrity wastefully
 - bounded lcore

Using DPDK needs many knowledge and tuning technique.
Susanow wrap these problems.


Benefit to use Mult/Many Cores
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Divide Granularity
 - Divide heavy process  -> GOOD
 - Divide light process  -> BAD dividion overhead is big

So, it is good to dynamic fix hotspot to CPU pinnning.

Divide Process Pattern
 - Divide each ports      ->  Stable each ports
 - Divide each functions  ->  Stable each functions(Tx/Rx)
 - Divide each flows      ->  Stable each flows

Divide Point -> witch want to be the most stable
Allocating and freing memory segument on different NUMA-node seems to slow.



Keywords
^^^^^^^^
High Performance/Network/Packet/DPDK/Reconfigurable
/JIT/Assembler/C++11/Multi Core/Many Core/Multi Thread
/Tuning Less/Low latency/SW Optimization/Multiqueue NIC
/Soft Router/Soft Switch/Scalable



