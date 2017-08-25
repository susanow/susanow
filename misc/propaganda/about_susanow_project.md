
# Susanow計画について

Hiroki SHIROKURA (slank.dev@gmail.com)<br>
last update 2017.08.16

目的: プロジェクトを進める上で大事なポイントの整理や軌道修正.<br>
内容:
- プロジェクトのバックグラウンド
- プロジェクトの解決する問題, ターゲット
- プロジェクトの今後の展望

---
## バックグラウンド　

NFVは良い
- 設備コストの低下
- NFの迅速なデプロイ, 高い柔軟性
- サービスチェインによるオンデマンドなネットワーク構築

高性能NFVの実現が可能になった
- DPDK (No need to develop NOS)
- 汎用PCの高性能化 NUMA, Manycore
- 高性能NICの低価格化

DPDKの高い開発コスト
- Thread tuning, CPU assign
- 高性能化手順がふんわりとしている

---
## 解決する問題, ターゲット

実装するもの
- DPDKのスレッドチューニングの自動化
- no-VM NFV基盤
- これらの上で動くRouter

世間へのインパクト
- 性能変更が柔軟なRouter
- NFVサービスチェインをする新たなアプローチ
  VMを利用しないパターン

---
## ユーザに提供するもの, しないもの

すること
- VNF実装の独自のプログラミングモデルの提供
- それにそってVNFを実装すると実行時に必要に応じて
  ソフトウェアパイプラインを自動で最適化してVNFの性能を自動調節
- ユーザはNUMA nodeを考慮したプログラミングを行うことで
  自動チューニングの手助けをする

しないこと
- NIC構成の動的再構成はしない
  boot時にできるだけたくさんRingを用意してそれをソフトウェアでさばく

---
## 自動チューニングについて

利点
- 単純なRun2Completionはちゃんと自動スケールする

欠点
- 複雑なPipelineモデルの自動スケールがまだテストできていない
- チューニングの段階が広い (1,2,4,8コア割り当て)
- 自動チューニングが完全に動いてくれていない
	現状特定の状態までしかサポートしていない

---
## SMP l3fwd の対応に関して

以下のようにしてMultiQueueを監視する.
ただし若干の条件がある.

<p><img src="./img/fig1.jpg" width="100%"/></p>

---
## NUMA l3fwd の対応に関して

<p><img src="./img/fig2.jpg" width="100%"/></p>

---
## NUMA Lagopus の対応に関して

- 現在対応中
- どのようにスレッドを増やしていけるかがまだまとまっていない
- Lagopusに対応することができれば, Pipelineモデルの一般的な
  自動最適化ができそう
- Lagopusは対応できればする

---
## スレッド追加の完成度

- 2^n多重での自動チューニングに対応 <br>
  1,2,4,8,16,....
- Multi Ring構造に対応

---
## Emulated Multi Queue NIC

- 物理NICを動的に再構成するのは現実的に不可能
- 事前に多めにQueueuを用意しておく.
  それをSoftwareでうまく振り分けていく
	-> Overheadはどの程度か...
	-> Threadを増やせば増やすほど,Soft抽象化が減るので大丈夫

---
## Evaluation

- 動的最適化のEvel
	- l3fwd
- Function ChainingのEval
	- i.e. l3fwd + packet-capture
	- OvS, SR-IOV

---
## 今後の展望

うわさでまだ試していない内容
- Seaster + Schelaのマルチスレッド最適化が良い?
- HTが意味ないことの確認
- Inter Processorの性能低下　
- Click Moduler Routerのプログラミングモデルはさんこうになる



