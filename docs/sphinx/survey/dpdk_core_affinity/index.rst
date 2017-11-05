
DPDK Core Affinity Mechanism
============================

write soon

- KVM側でvCPUを増やしたり減らしたりはできるっぽい
        - OS側がそれをうまく受け取るかは別として
- DPDK側は``pthread_setaffinity``でコアを奪っているので
  そのぶんの実装を少し改良すれば良さそう


References
----------

- liva_cpuhotplug_

.. _liva_cpuhotplug: http://raphine.hatenablog.com/entry/2017/07/13/004011


