
# RSS Evaluation

- Hiroki SHIROKURA
- slank.dev [at] gmail.com
- 2017.10.5

![fig1](./screenshot.png)

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
#deinf DELAY_N 100
#define DERAY \
  do { \
  	size_t n=0; \
  	for (size_t i=0; i<DELAY_N; i++) n++; \
  } while (false);
```

```
Size: [Byte]
TP  : [Mbps]

DELAY_N=100
size            0064   0128   0256   0512   1024
--------------------------------------------------
1Q(1 lcores)    3030   5324   9902   9998   9998
2Q(2 lcores)    5800   9998   9998   9998   9998
4Q(4 lcores)    8832   9998   9998   9998   9998
BaseLine        9998   9998   9998   9998   9998

DELAY_N=200
size            0064   0128   0256   0512   1024
--------------------------------------------------
1Q(1 lcores)    1664   2933   5470   9998   9998
2Q(2 lcores)    3332   5866   9998   9998   9998
4Q(4 lcores)    6685   9998   9998   9998   9998
BaseLine        9998   9998   9998   9998   9998

DELAY_N=300
size            0064   0128   0256   0512   1024
--------------------------------------------------
1Q(1 lcores)    1168   2061   3836   7402   9998
2Q(2 lcores)    2315   4080   7606   9998   9998
4Q(4 lcores)    4666   8277   9998   9998   9998
BaseLine        9998   9998   9998   9998   9998

DELAY_N=400
size            0064   0128   0256   0512   1024
--------------------------------------------------
1Q(1 lcores)     894   1575   2939   5692   9998
2Q(2 lcores)    1774   3128   5840   9998   9998
4Q(4 lcores)    3552   6288   9998   9998   9998
BaseLine        9998   9998   9998   9998   9998
```

## Reference 

- http://pktgen-dpdk.readthedocs.io/en/latest/index.html
- https://www.miraclelinux.com/tech-blog/dpdk-open-vswitch-study-5


