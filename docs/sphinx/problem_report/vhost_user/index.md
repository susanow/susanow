
# Vhost Userがドキュメント通りに動いてくれない

- Hiroki SHIROKURA (slank.dev[at]gmail.com)
- 2017.10.17

結論から言うと, ドキュメントをよく読んでいなかっただけなのだが,
結構陥り安い問題だとおもうので, 報告.

## DPDKのドキュメント通りに起動しない問題

[3]にある通りの問題が起きた.
DPDKのVhost Sample ApplicationのQemu実行引数ではVM起動時に以下のような
メッセージが出た.

```
qemu-system-x86_64: Failed initializing vhost-user memory map,
consider using -object memory-backend-file share=on
qemu-system-x86_64: unable to start vhost net: 11: falling
back on userspace virtio
```

そこで``Failed initializing vhost-user memory map``でググるとfd.ioの
MLがヒットする. ある程度, その内容の通りに引数を修正したらうまくいった.

Diffはこんな感じ
```
  $QEMU \
	 -enable-kvm -cpu host \
-  -m 1024M \
-  -object memory-backend-file,id=mem,size=4096M,mem-path=$HPPATH,share=on \
-  -mem-prealloc \
-  -numa node \
+  -m 512 \
+  -object memory-backend-file,id=mem,size=512M,mem-path=$HPPATH,share=on \
+  -numa node,memdev=mem -m 512 -mem-prealloc -debugcon file:debug.log \
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
  -m 512 \
	-object memory-backend-file,id=mem,size=512M,mem-path=$HPPATH,share=on \
  -numa node,memdev=mem -m 512 -mem-prealloc -debugcon file:debug.log \
  -hda $HDAPATH \
  -boot c -vnc :0,password -monitor stdio \
  \
  -chardev socket,id=chr0,path=$SOCKPATH \
  -netdev vhost-user,id=net0,chardev=chr0,vhostforce,queues=1 \
  -device virtio-net-pci,netdev=net0
```

## Multi Queueに対応しない問題

vhost_userでmulti queueを使いたい場合, ``virtio-pci``か
``igb_uio.ko``(DPDKでビルドされる)しか対応していない.

参考文献[6]に詳しいことが書いてある. (OpenstackとIntelのやつにのみ答えがある)


## 参考文献

- [3] ML [vpp-dev] Qemu error vhost-user Failed initializing vhost-user memory map
	- 最初は参照1を参考にした, でうまくいかなくて, 参考2との差分をいじったらうまくいった
	- 参考1: http://dpdk.org/doc/guides/sample_app_ug/vhost.html
	- 参考2: https://lists.fd.io/pipermail/vpp-dev/2017-July/005796.html

- [6] vHost User Multiqueue Configuration and Use
	- IntelとOpenstackには``igb_uio``が必要だと書いてある
	- https://docs.openstack.org/draft/ja/networking-guide/config-ovs-dpdk.html
	- https://software.intel.com/en-us/articles/configure-vhost-user-multiqueue-for-ovs-with-dpdk
	- http://docs.openvswitch.org/en/latest/topics/dpdk/vhost-user/
	- https://wiki.qemu.org/Documentation/vhost-user-ovs-dpdk


