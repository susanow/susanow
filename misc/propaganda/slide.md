
# Susanow:<br>環境に対して自動最適化する<br>高性能通信基盤

- 高速PCルータ研究会2017.08 in 田町
- Hiroki SHIROKURA @slankdev slank.dev@gmail.com

===

## Introduction

城倉 弘樹 (SHIROKURA Hiroki) aka slankdev <br>
http://slankdev.net

活動
- セキュリティキャンプ 2015~ 今年から講義を担当
- Cybozu-Lab-Youth [拡張可能なパケット解析ライブラリ]
- Cybozu-Lab-Youth [高性能TCP/IPネットワークスタック]
- IIJii アルバイト [DPDK, BPF, 高性能パケット処理]
- 未踏事業 [環境に対して自動で最適化する高性能通信基盤]

---
## Agenda

1. **Background, Motivation, Summary**
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

ネットワーク機能の仮想化(ソフトウェア化)
- 汎用機によりコストダウン
- 迅速なデプロイ,機能追加
- 複数VNFを単一ホストにデプロイ (後述)

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
## OvS-DPDK architecture

- OVS用にいくつかのCPUを使用する
- {sum of vCPU} > {num of cores}になったら, vm\_entry, vm\_exitの数が上昇?
- DPDKのCPUpinningの効果が低減
- VM上で動くVNFが一般的なDPDK VNFならまとめて管理をできる

<p><img src="./img/fig7.ovs.png" width="100%"/></p>


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

---
## Agenda

1. Background, Motivation, Summary
2. **System Design**
3. Demonstration

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
## 現状の問題点
- Kamueeのようなパターンはどうやって動的最適化するか
- 一般:「チューニングにNIC構成変更の必要があるVNFは最適化可能か」
- vNICとしてアブストラクションするとOvS見たいにpNICのスレッドが必要になる.
  以下のpNICを制御しているコアのように

<p><img src="./img/fig7.ovs.png" width="100%"/></p>

---
## 評価, 案検討中

- どのような構成でどうやって検証をすればよいのか
- @yasu 卒業研究と同スペックでNetVMやOVSと勝負ができるかも
- NFVサービスチェインが迅速にいじれて高性能なことを売りにしたい
- できれば40coreくらい使いたい...

---
## デモ

- スレッドチューニングの動的最適化
- VNFを新たにデプロイした場合の自動最適化エンジン



