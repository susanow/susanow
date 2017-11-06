
DPDK Core Affinity Mechanism
============================

write soon

- KVM側でvCPUを増やしたり減らしたりはできるっぽい
- OS側がそれをうまく受け取るかは別として
- DPDK側は``pthread_setaffinity``でコアを奪っているので
  そのぶんの実装を少し改良すれば良さそう

``rte_config``が以下のようになっていて,``lcore_count``に代入している部分を
中心に調べていくのが方針.

コアの固定に関しては``pthread_setaffinity_np()``でコアのアサインを
してるはずなので、そこも詳しく調べる.
DPDKでは``rte_thread_set_affinity()``というのがあるくて、それが味噌ぽいなあ.

```
// lib/common/include/rte_eal.h:80

struct rte_config {
	u32 master_lcore;
	u32 lcore_count;
	rte_lcore_role_t lcore_role[RTE_MAX_LCORE];
	rte_proc_type_t process_type;
	rte_mem_config mem_config;
};
```

References
----------

- dpdk ealのドキュメント http://dpdk.org/doc/guides/prog_guide/env_abstraction_layer.html
- liva死のブログ: http://raphine.hatenablog.com/entry/2017/07/13/004011


