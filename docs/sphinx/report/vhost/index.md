
# Vhost 情報収集

- Hiroki SHIROKURA (slank.dev[at]gmail.com)
- 2017.10.12

## 仮想NICのパターン

- e1000エミュレーション
- virtio-net
- vhost-net
- vhost-user

## 仮想NICのボトルネック

- メモリコピー
- コンテキストスイッチ
	- VMX non-root,root切り替え命令 (VM Entry/Exit)
	- OS(Guest, Host)のコンテキストスイッチ

## DPDKのドキュメント通りに起動しない問題

[3]にある通りの問題が起きた.
DPDKのVhost Sample ApplicationのQemu実行引数ではVM起動時に以下のようなメッセージが出た
```
qemu-system-x86_64: Failed initializing vhost-user memory map, consider using -object memory-backend-file share=on
qemu-system-x86_64: unable to start vhost net: 11: falling back on userspace virtio
```

そこで``Failed initializing vhost-user memory map``でググるとfd.ioの
MLがヒットする. ある程度, その内容の通りに引数を修正したらうまくいった.

Diffはこんな感じ
```
  $QEMU \
         -enable-kvm -cpu host \
-        -m 1024M -object memory-backend-file,id=mem,size=4096M,mem-path=$HPPATH,share=on \
-        -mem-prealloc \
-        -numa node \
+        -m 512 -object memory-backend-file,id=mem,size=512M,mem-path=$HPPATH,share=on \
+        -numa node,memdev=mem -m 512 -mem-prealloc -debugcon file:debug.log \
         -hda $HDAPATH \
         -boot c -vnc :0,password -monitor stdio \
```

修正後の実行スクリプトを示す
```
#!/bin/sh

QEMU=qemu-system-x86_64
HDAPATH=/home/slank/qemu/u1604.qcow2
SOCKPATH=/tmp/sock0
HPPATH=/mnt/huge_kvm

$QEMU \
  -enable-kvm -cpu host \
  -m 512 -object memory-backend-file,id=mem,size=512M,mem-path=$HPPATH,share=on \
  -numa node,memdev=mem -m 512 -mem-prealloc -debugcon file:debug.log \
  -hda $HDAPATH \
  -boot c -vnc :0,password -monitor stdio \
  \
  -chardev socket,id=chr0,path=$SOCKPATH \
  -netdev vhost-user,id=net0,chardev=chr0,vhostforce,queues=1 \
  -device virtio-net-pci,netdev=net0
```

## 問題: vcpuの物理コアに対する貼り付けかたがわからない

上記の内容で動かしている状態だと``qemu-system-**``でやっているので、
virshが使えなくて困ったので, VMをlibvirtで用意し直して作業を再開.
だれかqemuから起動したVMでリアルタイムにCPUアサインができる方法を
知っていたら教えてください....

結局実験で使うVMは以下のようなHW構成になった (XMLを示す)

```
```

## 性能計測のために考えたこと

まず以下を固定条件とする
- vcpupinをして論理コアを占有
- NUMAは考慮しない (smpのみ)

本検証では以下のパターンを用いて検証した.


## 参考文献

- [1] KVM as The NFV Hypervisor
	- Jun Nakajima (intel)
	- vhostuserのshmについて良い図が乗っている
	- http://www.linux-kvm.org/images/8/87/02x09-Aspen-Jun_Nakajima-KVM_as_the_NFV_Hypervisor.pdf

- [2] Introduction to Intel DPDK
	- Tetsuya Mukawa (IGEL Co.,Ltd)
	- 一番わかりやすい説ある (三種類全て説明している)
	- https://elinux.org/images/8/80/Introduction_to_Intel_DPDK_v2.pdf

- [3] ML [vpp-dev] Qemu error vhost-user Failed initializing vhost-user memory map
	- 最初は参照1を参考にした, でうまくいかなくて, 参考2との差分をいじったらうまくいった
	- 参考1: http://dpdk.org/doc/guides/sample_app_ug/vhost.html
	- 参考2: https://lists.fd.io/pipermail/vpp-dev/2017-July/005796.html

- [4] vhostの実験スクリプト
	- vhost-user仮想NICを一つ /tmp/sock0に用意するプログラム
	- DPDKアプリはパケットを受信したらhexdumpするだけ
	- VM起動スクリプトもある
	- https://github.com/susanow/libdpdk_cpp/tree/master/examples/vhost


