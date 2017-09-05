
# Susanow:<br>環境に対して自動最適化する<br>高性能通信基盤

- Hiroki SHIROKURA @slankdev slank.dev@gmail.com
- powered by IPA-MITOU-program

===

## Introduction

城倉 弘樹 (SHIROKURA Hiroki) aka slankdev <br>
http://slankdev.net

活動
- セキュリティキャンプ 2015~
- Cybozu-Lab-Youth [拡張可能なパケット解析ライブラリ]
- Cybozu-Lab-Youth [高性能TCP/IPネットワークスタック]
- IIJii アルバイト [DPDK, BPF, 高性能パケット処理]
- 未踏事業 [環境に対して自動で最適化する高性能通信基盤]

---
## Agenda

1. Background, Motivation, Summary
2. System Design
3. Demonstration


---
## DPDKの影響と高い開発コスト

- DPDKによりLinux上での高性能通信が可能
- しかし高い開発コスト.
- 高性能通信の仮想化は簡単には手に入らない
- DPDK VNFの性能はCPU数でスケール
- Run-to-Completion, Pipeline

キーワード<br>
ソフトウェアパイプライン/NICコンフィグ/ゼロコピー/排他制御

**最適なスレッドデザインで広帯域,程遅延を実現せよ**
<p><img src="./img/fig2.thread.png" width="100%"/></p>

---
## Network Function Virtualization

ネットワーク機能を仮想化技術で実現すること.
本プロジェクトはいくつかにフォーカスして説明する.

利点
- コストダウン
	- 値段(汎用サーバ << 専用HW)
	- 保守運用(統一的インターフェース,自動化)
- 迅速
	- サービスの拡大/縮小
	- デプロイ,機能拡張

可能になった背景
- 高速マルチコアCPUを搭載した高性能なパケット処理が可能
- クラウドインフラは、リソースの可用性と使用を向上させる方法を提供
- 管理,制御APIのオープン化
- 業界標準の大容量サーバ

課題
- Portability/Interoperability
- Performance Trade-Off
- Manage & Orchestration
- Automation

<p><img src="./img/fig1.nfv.png" width="100%"/></p>

---
## Function Chaining

- 汎用機で複数のNFを繋げてうごかす
- 迅速にサービスを拡大/縮小
- ex) Router → Router+IDS
- 多くがVMを用いてVNFを実現
- DPDKとVMの相性は?

<p><img src="./img/fig3.chaining.png" width="100%"/></p>

---
## NFV with VM

利点: 高いセキュリティ, 抽象性

欠点:
- VMオーバヘッド
- VMのコンピュータ構成の変更(vCPU数を動的に変化)
- VMのチューニング, VNFのチューニング
- DPDK性能はCPU数とNIC構成でスケール

<p><img src="./img/fig4.vm.png" width="100%"/></p>

---
## ex) OvS-DPDK architecture

- OVS用にいくつかのCPUを使用する
- {sum of vCPU} > {num of cores}になったら, vm\_entry, vm\_exitの数が上昇?
- DPDKのCPUpinningの効果が低減
- VM上で動くVNFが一般的なDPDK VNFならまとめて管理をできる

<p><img src="./img/fig7.ovs.png" width="100%"/></p>


---
## 現状のアーキテクチャの問題点

- VMによるNFVによりパフォーマンス低下
	- VMの性能変更より迅速に性能を変更したい
	- VMオーバヘッドは考えていない
- 様々な情報は色々なタイミングで決定する
	- 企画次に決まる情報
	- デプロイ次に決まる情報
	- 実行中に決まる情報
- 高度に仮想化がすすみつつある現代ではHSPCRを実装しただけではだめ
- それを利用するフィールドの整備まで行わなければならない

---
## 本テーマの貢献とインパクト

- DPDK VNFの動的自動スケーリング
- NFV基盤として実装し, Chained VNFの最適化

環境に対して自動最適化する高性能通信基盤
- 環境: トラフィック状態, コンピュータリソース, etc
- 自動最適化: スレッドチューニングで全体スループットを最高性能化
- 高性能: スループットに重点, 10GbEx8, 40GbEx2,
- 通信基盤: notVM NFV Infrastructure

ターゲットは10GbE x4-8, 40GbE x4くらい<br>
まだ未完成. BCP(BestCurrentPractice)をお話します

```
コンピュータリソース100%10G
A33%10Gbps B33%05Gbps C33%20Gbps
A33%10Gbps B66%10Gbps C16%10Gbps
とする、アルゴリズムを開発する
```

ベンダのエコシステムを利用することができる. (要参考文献)

Susanow計画のワークロード
- スレッド最適化による環境に対して自動最適化する高性能通信基盤
- 上記を複数NFに適用させ, ネットワークスライスを最適化するNFV基盤
- 複数ノードを追加することにより無限にスケールするオーケストレータエージェント
- 開発した基盤上で動くNF複数種類 (VNFリポジトリ)
	- DPI, Router, FW, QoS
	- 帯域混雑時でも緊急電話に対応できる可用性99.9%のVoIP

開発者の本音
- NFVサービスチェインが迅速にいじれて高性能なことを売りにしたい
- できれば40coreくらい使いたい...


---
## System Architecture

- DPDKのラップ
	- スレッド管理 (Native/Green)
	- port管理 (Physical/Virutual)
	- timer管理
	- その他
- VNF管理: 自動最適化エンジン
- フロントエンド (VTY)

<p><img src="./img/fig5.susanow.png" width="50%"/></p>

---
## DPDKのラップ: Thread, Timer, Port (実装の話)

- スレッド管理 (Native/Green)
	- lthreadのスケジューラアクセスをシリアライズ
- timer管理
	- タイマーオブジェクトクラスとしてラップ
- port管理 (Physical/Virutual)
	- 仮装ポート,物理ポートのインターフェースを統一
	- 性能計測機能を追加

<p><img src="./img/fig6.wrapdpdk.png" width="100%"/></p>

---
## VNFの設計

一般的に必要な要件: コンピュータリソースを与えると性能向上ができる

VNFに必要な機能
- stageの集合

Stageに必要な機能
- 多重化する機能(inc)
- 逆多重化機能(dec)
- 統計情報取得機能(性能,状態)
- 入出力ポート定義(物理,仮想)

---
## 自動最適化概要

1. 発火フェーズ
	- VNFを追加したり減らしたりするタイミングでOptimization
	- トラフィックが増えたり, 減ったりするタイミングでOptimization
	- タイマーで一定期間ごとに性能チェック. 低下したタイミングで発火
2. 発見フェーズ (環境情報より発見)
	- NICのスループット
	- スレッド状態(launch数,遅延)
	- 空きCPUの個数
3. 修正フェーズ
	- スレッドの多重度
	- NICチューニング

---
## 全体最適化手順
**step1** <p><img src="./img/opt1.png" width="100%"/></p>
**step2** <p><img src="./img/opt2.png" width="100%"/></p>
**step3** <p><img src="./img/opt3.png" width="100%"/></p>

---
## VNF最適化手順
**step1** <p><img src="./img/vnfopt1.png" width="100%"/></p>
**step2** <p><img src="./img/vnfopt2.png" width="100%"/></p>
**step3** <p><img src="./img/vnfopt3.png" width="100%"/></p>

---
## 発火,発見,修正フェーズのアルゴリズム

```
void optimize_vnfchaing(vnf[] vnfs) {
	for (i=0; i<nb_vnfs; i++) {
		if (vnfs[i].has_perf_reduction()) {
			optimize_stage(vnfs[i]);
			return ;
		}
	}
}
void optimize_stage(vnf nf) {
	for (i=0; i<nb_stages; i++) {
		if (nf.stage[i].has_perf_reduction()) {
			nf.stages[i].inc();
			return ;
		}
	}
}
```

---
## Evaluation

- スレッド最適化による動的な性能変更
- SFCの最適化によるネットワークスライスの動的な性能変更
	- 非NUMA構成
	- NUMA構成
- 複数ノードでのクラスタリングの動的な性能変更
- 実装したVNFの個別な性能変更の確認
- これらを可能にするVNFのプログラミングモデル
- NFVを管理する独自プロトコルとそのAPI



