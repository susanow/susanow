
.. _nonvm_nfvi:

nonVM NFV Infrastructure
========================

SusanowはVMを用いないNFV基盤(NFVi)を提供している.
VNFはVMにカプセルされなく, ホスト上で直接実行される.

.. todo:: このページを書く
  VMオーバヘッドの問題提起とゼロコピーでのパケット交換を説明する.
  性能面だけでなく, セキュリティ面のリスクも取り入れて説明をする.

susanowに含まれているNFViは以下の機能を提供する

- リンクされたVNFクラスからVNFのインスタンスを生成
- VNFの仮装ポートを設定. pciデバイスやtapデバイス, sppデバイスを指定可能.
- VNFの動作設定が可能
- VNFのデプロイ, アンデプロイが可能
- NVFiのインターフェースはコードベースで操作可能
- NFViのインターフェースはgRPCで操作可能
- NFViのインターフェースは内臓CLIシェルで操作可能
- gRPCを用いてD2engineと通信し, VNFの動的スケジューリング可能


