
# Deploy NVFi

ここではNFViのデプロイ方法についてソースコードレベルで解説を行う.
以下のサンプルコードをコンパイルし, 実行することでsusanowの提供する
NFViをデプロイすることができる. 本ページでは以下の方法を解説する.

- NFViのデプロイ
- VNF poolにVNFを追加
- VNFのデプロイ

## NFViのデプロイ

以下のコードをコンパイルして実行する.

```
#include <ssn_nfvi.h>

int main(int argc, char** argv)
{
  ssn_nfvi nfvi(argc, argv);
  nfvi.deploy();
}
```

以上の実装で完了である. デプロイ後はVTYを用いて, システムの操作を行う.
ssnホストのIPアドレスは``10.0.0.3``とする.


```
$ telnet 10.0.0.3 9999
```

## ポートの追加

```
> list portpool
- dpdk0
- dpdk1

> add port vhostuser vhost0 /tmp/sock0 2
> list portpool
- dpdk0
- dpdk1
- vhost0
```

## VNFのデプロイ

```
> list vnfpool
- l2fwd_2port
- l3fwd_2port
- acl_2port

> list portpool
- dpdk0
- dpdk1
- virt0
- virt1

> vnf l2fwd_2port new <instance-name>
> vnf <instance-name> attachport 0 dpdk0
> vnf <instance-name> attachport 1 dpdk1
> vnf <instance-name> deploy
```


