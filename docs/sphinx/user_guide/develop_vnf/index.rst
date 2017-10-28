
VNF開発方法
===========

ここではsusanowを用いた新規なVNFの開発方法を説明していく.
この例ではシンプルなL2FWDを開発する方法を示していく.
ここで説明するVNFは以下の機能をサポートする

- NUMA Aware
- D2による自動最適化の対応

VNFを開発する流れは以下に従う.

1. VNFクラスを継承し, クラスを実装する
2. きめられた手順でデプロイする

VNFクラスを継承し, クラスを実装する
-----------------------------------

write soon.

きめられた手順でVNFをデプロイする
---------------------------------

この方法は二種類存在する.
ssn_nvfiを使用した場合, 必要な手順をある程度隠蔽してデプロイすることが
できるが, 実際にコードベースでデプロイをする場合は少し手順が必要である.
D2のエンジンはこの手順を用いてVNFの自動スケールを行う.

以下は実際のサンプルコードである.

.. code-block:: c++

  ssn_vnf_port_dpdk* dpdk0 = new ssn_vnf_port_dpdk(0, 4, 4);
  ssn_vnf_port_dpdk* dpdk1 = new ssn_vnf_port_dpdk(1, 4, 4);

  ssn_vnf_l2fwd vnf("vnf0");
  vnf.attach_port(0, dpdk0);
  vnf.attach_port(1, dpdk1);

  vnf.reset_allport_acc();
  vnf.set_coremask(0, 0x02);
  vnf.deploy();
  vnf.undeploy();

このようにしてデプロイを行う. 手順を形式的に書くと以下の手順である.

1. vportインスタンスの生成
2. vnfインスタンスの生成
3. vnfに対して, vportをアタッチ
4. vportのaccessorをリセット
5. vnfのblockにコアマスクを設定
6. vnfのdeploy関数を実行



