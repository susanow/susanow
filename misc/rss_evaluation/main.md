
# RSS Evaluation

- Hiroki SHIROKURA
- slank.dev [at] gmail.com
- 2017.10.5

## Environment

- CPU: Intel(R) Core(TM) i7-6700K CPU @ 4.00GHz
- HyperThreading: Enabled (4C/8T)
- RAM: 16294776 KiB
- DPDK: 17.05


## Traffic

- pktgen 3.2.10 (DPDK 17.08-rc0)

```
# sudo ./app/x86_64-native-linuxapp-gcc/pktgen \
        -- \
        -P \
        -m "[1-4].0-1"
```

```
> enable all range

> range all size start 64
> start 0
> stop  0

> range all size start 128
> start 0
> stop  0

...
```

## Data

```
while (true) {
  rte_mbuf* mbufs[32];
	n_recv = rx_burst(pid, qid, mbufs, 32);
	for (i=0; i<n_recv; i++) {
		DERAY();
		tx_burst(pid^1, qid, &mbufs[i], 1);
	}
}

```

```
#define DERAY \
  do { \
  	size_t n=0; \
  	for (size_t i=0; i<100; i++) n++; \
  } while (false);
```

```
Size: [Byte]
TP  : [Mbps]

size            0064   0128   0256   0512   1024
--------------------------------------------------
1Q(1 lcores)    3030   5324   9902   9998   9998
2Q(2 lcores)    5800   9998   9998   9998   9998
4Q(4 lcores)    8832   9998   9998   9998   9998
BaseLine        9998   9998   9998   9998   9998
```

## Reference 

- http://pktgen-dpdk.readthedocs.io/en/latest/index.html
- https://www.miraclelinux.com/tech-blog/dpdk-open-vswitch-study-5


