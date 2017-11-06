
# DPDK rte\_ring 性能計測

## 目的

DPDKでNFを実装する場合に使用する``rte_ring``の性能を理解すること

## 内容

- enqueueとdequeueを実行した場合のレイテンシを計測する
- 1 shotでのenq/deqとbulkでのenq/deqの性能計測を行う
- enq/deqするパケットが性能に影響するかを確認する
- レイテンシの計測にはrdtscpを使用する
  rdtscpの呼び出しオーバヘッドはないものとする. (Intel SDMを見ないと...)
- レイテンシの計測単位はclock


## 試験項目

- 1shot enq/deqのレイテンシ計測                    (test1)
- bulk enq/deqのレイテンシ計測                     (test2)
- 1shot enq/deqでpacket sizeを変えたときの性能変化 (test3)


## プログラム

test1

```
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <slankdev/extra/dpdk.h>
#include <slankdev/system.h>

const uint8_t packet[64] = {0xee};

int main(int argc, char** argv)
{
  slankdev::dpdk_boot(argc, argv);
  struct rte_mempool* mp = slankdev::mp_alloc("RXMBUFMP");

  struct rte_mbuf* mbuf = rte_pktmbuf_alloc(mp);
  slankdev::set_mbuf_raw(mbuf, packet, sizeof(packet));

  struct rte_ring* ring = slankdev::ring_alloc("WK0", 1024);
  slankdev::safe_ring_enqueue(ring, mbuf);

  uint64_t tmp;
  uint64_t enq_sum = 0;
  uint64_t deq_sum = 0;
  uint32_t lcore_id = -1;
  size_t loops = 10000000000;
  printf("loops %lu \n", loops);
  for (size_t count=0; count<loops; count++) {
    tmp = slankdev::rdtscp(&lcore_id);
    slankdev::safe_ring_dequeue(ring, (void**)&mbuf);
    deq_sum += slankdev::rdtscp(&lcore_id) - tmp;

    tmp = slankdev::rdtscp(&lcore_id);
    slankdev::safe_ring_enqueue(ring, mbuf);
    enq_sum += slankdev::rdtscp(&lcore_id) - tmp;
  }
  printf("\n\n");
  printf("%10s, %10s, %10s\n", "", "enq[clk]", "deq[clk]");
  printf("%10s, %10lu, %10lu\n", "1shot", enq_sum/loops, deq_sum/loops);
}
```

test2

```
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <slankdev/extra/dpdk.h>
#include <slankdev/system.h>
#include <slankdev/string.h>

const uint8_t packet[] = {0xee};

void test(size_t BURSTSIZE)
{
  struct rte_mempool* mp = slankdev::mp_alloc("RXMBUFMP");

  struct rte_mbuf* mbufs[BURSTSIZE];
  for (size_t i=0; i<BURSTSIZE; i++) {
    mbufs[i] = rte_pktmbuf_alloc(mp);
    slankdev::set_mbuf_raw(mbufs[i], packet, sizeof(packet));
  }

  struct rte_ring* ring = slankdev::ring_alloc("WK0", 1024);
  slankdev::safe_ring_enqueue_bulk(ring, (void**)mbufs, BURSTSIZE);

  uint64_t tmp;
  uint64_t enq_sum = 0;
  uint64_t deq_sum = 0;
  uint32_t lcore_id = -1;
  size_t loops = 10000000000; /* 10^10 */
  for (size_t count=0; count<loops; count++) {
    tmp = slankdev::rdtscp(&lcore_id);
    slankdev::safe_ring_dequeue_bulk(ring, (void**)mbufs, BURSTSIZE);
    deq_sum += slankdev::rdtscp(&lcore_id) - tmp;

    tmp = slankdev::rdtscp(&lcore_id);
    slankdev::safe_ring_enqueue_bulk(ring, (void**)mbufs, BURSTSIZE);
    enq_sum += slankdev::rdtscp(&lcore_id) - tmp;
  }
  printf("%10zd, %10lu, %10lu\n", BURSTSIZE, enq_sum/loops, deq_sum/loops);

  slankdev::rte_pktmbuf_free_bulk(mbufs, BURSTSIZE);
  rte_ring_free(ring);
  rte_mempool_free(mp);
}

int main(int argc, char** argv)
{
  slankdev::dpdk_boot(argc, argv);

  printf("\n\n");
  printf("%10s, %10s, %10s\n", "bulksize", "enq[clk]", "deq[clk]");
  test(1);
  test(2);
  test(4);
  test(8);
  test(16);
  test(32);
}
```

## 結果

test1

10^10回の試行の平均値を計算
```
  bulksize,   enq[clk],   deq[clk]
         1,        139,        135
         2,        142,        137
         4,        149,        144
         8,        164,        161
        16,        202,        196
        32,        273,        266
```

test2
```
```

test3は詳細にしらべていない.

## Memo

- enq/deqするパケットサイズとenq/deqの性能に関係はなかった.
  パケットアクセスでなく、mbufのポインタをenq/deqしているので当たり前では
	あるが、一応調べた。



