
Susanow計画とは
===================================

Susanow(スサノオ)計画は高度に動的な高性能NFVプラットフォームを開発する
研究開発プロジェクトです.
本計画ではVNF開発のためのSDK, VNFをデプロイするNFVi(NFV基盤), サンプルとして
動作するVNFを開発しています.
プロジェクトの背景は :doc:`background/index` を参照してください.

VNFのチューニングの自動化
^^^^^^^^^^^^^^^^^^^^^^^^^
Susanow計画のもっとも大きな特徴はVNFのスレッドレベルの高性能化をVNFの実装側
で行う(プログラマが行う)のではなく, NFViが行う点にあります.
それにより, プログラマーがSusanow上で動くVNFを一度記述すればそれをさまざまな
環境に最適な性能で動作させることができます. (例: 1GbEから40GbEなど)

non-VMなNFV
^^^^^^^^^^^
VMを用いたNFVの構築はPortabilityやCompatibilityが良い反面, 様々なオーバヘッドが
あります. 例としてIntel VT-xのVMX-root/non-rootのモード切り替えには約1000
clockほどかかってしまいます. 他にもVMの仮想NICのオーバヘッドなど様々な性能課題
があるので, SusanowではVMMを用いない形でNFVを実現しています.
具体的にはSusanow独自のプログラミングモデルでVNFを開発することで
インターフェースを統一し, 移植性を高めています.
しかしOpen Networkingを実現するために, 今後はVM上のNetwork Functionのデプロイ
もサポートする予定です.

DPDKを用いた高性能なパケットフォワード
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

SusanowではデータプレーンにDPDKを採用しています. DPDKのおかげでIAサーバ上で
Linuxシステムと共存させながら高性能通信環境を実現することができています.

システムアーキテクチャ
^^^^^^^^^^^^^^^^^^^^^^

.. image:: architecuture.png

上記の図はSusanowの利用構成の一例を示しています. この例ではNet Function Aと
Net Function Bという二つのVNFがデプロイされていて, それぞれコアが4, 12コアずつ
割り当てられています. 各VNFは仮想NIC(vNIC)を2つずつ持っており, サービスチェイン
がされています.

より詳細な情報
^^^^^^^^^^^^^^

.. toctree::
  :maxdepth: 1

  background/index
  features/index
  progress


