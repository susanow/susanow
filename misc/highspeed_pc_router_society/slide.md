
# Susanow:<br>環境に対して自動最適化する<br>高性能通信基盤

- 高速PCルータ研究会2017.08 in 田町
- Hiroki SHIROKURA @slankdev slank.dev@gmail.com

----
## Introduction

城倉 弘樹 (SHIROKURA Hiroki) aka slankdev <br>
http://slankdev.net

活動
- セキュリティキャンプ 2015~ 今年から講義を担当
- Cybozu-Lab-Youth [拡張可能なパケット解析ライブラリ]
- Cybozu-Lab-Youth [高性能TCP/IPネットワークスタック]
- IIJii アルバイト [DPDK, BPF, 高性能パケット処理]
- 未踏事業 [環境に対して自動で最適化する高性能通信基盤]

Agenda
- Background, Motivation, Summary
- System Design
- Demonstration

----
## Network Function Virtualization

ネットワーク機能の仮想化(ソフトウェア化)
- 汎用機によりコストダウン
- 迅速なデプロイ,機能追加
- 複数VNFを単一ホストにデプロイ

<p><img src="./img/fig1.nfv.png" width="100%"/></p>

----
## DPDKの影響と高い開発コスト

- DPDKによりLinux上での高性能通信が可能
- しかし高い開発コスト.
- 高性能通信の仮想化は簡単には手に入らない
- DPDK VNFの性能はCPU数でスケール

キーワード<br>
ソフトウェアパイプライン/NICコンフィグ/ゼロコピー/排他制御

**最適なスレッドデザインで広帯域,程遅延を実現せよ**
<p><img src="./img/fig2.thread.png" width="100%"/></p>

----
## Function Chaining
- 汎用機で複数のNFを繋げてうごかす
- 迅速にサービスを拡大/縮小
- ex) Router → Router+IDS
- 多くがVMを用いてVNFを実現
- DPDKとの相性は?

<p><img src="./img/fig3.chaining.png" width="100%"/></p>

----
## Susanowの貢献とインパクト

- DPDK VNFの動的自動スケーリング
- NFV基盤として実装し, Chained VNFの最適化

環境に対して自動最適化する高性能通信基盤
- 環境: トラフィック状態, コンピュータリソース, etc
- 自動最適化: スレッドチューニングで全体スループットを最高性能化
- 高性能: スループットに重点, 10GbEx8, 40GbEx2,
- 通信基盤: notVM NFV Infrastructure

まだ未完成. BCP(BestCurrentPractice)をお話します

----
## なぜNonVM NFVi?
- VMオーバヘッド
- DPDK VNFの性能はCPU数とNIC構成でスケールする
- VMのvCPU数を動的に変化は難しい
- CPU管理部分を行う基盤を実装すればよい？
- ただセキュリティ機構については?

----
## 目的

- 背景: VMオーバヘッド上でのNVFは最高性能ではない
- 背景: DPDKのスレッド最適化は一般的に解決できそう
- 目的: 環境情報からスレッドのパイプラインを自動最適化
- 開発: 複数NFのデプロイするNFV基盤 (no VM)

- Callenge: 限界まで無駄のないソフトウェアパイプライン
- ターゲットは10GbE x4-8, 40GbE x4くらい
- コア数がどんどん上昇することを前提 (20core~256core)

----
## 設計1 全体概要

- DPDKのラップ
	- スレッド管理 (Native/Green)
	- port管理 (Physical/Virutual)
	- timer管理
	- ロックレスデータ構造
- VNF管理
- 自動最適化エンジン
- フロントエンド (WebUI/VTY)

----
## 設計2 DPDKのラップ

- スレッド管理 (Native/Green)
	- lthreadのスケジューラアクセスをシリアライズ
- port管理 (Physical/Virutual)
	- 仮装ポート,物理ポートのインターフェースを統一
	- 性能計測機能を追加
- timer管理
	- タイマーオブジェクトクラスとしてラップ
- ロックレスデータ構造
	- 性能計測機能を追加

----
## 設計3 VNFの実装

- NF開発者の実装部分
	- NF = {スレッドの集合, IOポート, 最低限のpipeline情報}
	- ソフトウェアパイプラインの最適化はしない
	- ソースをコンパイルしてSusanowにリンク
	- deploy関数でデプロイするだけ
	- VTY, WebUIから状態確認, 制御可能(にする予定)

----
## 設計4 仮装ポート

- NIC, rte\_ringを同一インターフェースに統一
- NICには統計情報機能があるが, ringにはないので, 拡張
- パイプラインの各ステージからアクセス

----
## 設計5 パフォーマンス管理部分

- 任意のブツのチェイニングを最適化する
	- 具体例
		-	NFのpipelinestageを最適化する
		- SFCのNFのチェイニングを最適化する

- NFのスループットを測る
	- 各パイプラインのスループットの最小値
		- 各パイプラインの性能計測に帰着!

----
## 設計6 自動最適化部分

1. 発火フェーズ
	- VNFを追加したり減らしたりするタイミングでOptimization
	- トラフィックが増えたり, 減ったりするタイミングでOptimization
2. 発見フェーズ (環境情報より発見)
	- NICのスループット
	- スレッド状態(launch数,遅延)
	- 空きCPUの個数
3. 修正フェーズ
	- スレッドの多重度
	- NICチューニング

----
## 評価, 案検討中

- どのような構成でどうやって検証をすればよいのか
- @yasu 卒業研究と同スペックでNetVMやOVSと勝負ができるかも
- NFVサービスチェインが迅速にいじれて高性能なことを売りにしたい
- できれば40coreくらい使いたい...

----
## デモ

- スレッドチューニングの動的最適化
- VNFを新たにデプロイした場合の自動最適化エンジン

----
## まとめ1 なぜこのようなことが可能になるのか

各ステージの性能の定義を結構無理やりやった。
一般的な構造ではない
ネットワークスタックのような通り抜けしない構造は想定していない
ルータやFirewallやIDSなどを想定
ターゲット設定を限定したことと、ユーザの開発部分をある程度
束縛することで実現した

----
## FAQ

- レイテンシどうするんですか
	- run to complitionのレイテンシ限界とはなにか
	- 複数スレッドを1lcoreにまとめるというアイディア


----
## 補足
### DPDK

- アイディア
	- Kernelのボトルネックを解消
	- ボトルネックの無いアーキテクチャを再設計
- アプローチ
	- CPU専有
	- ユーザランドドライバによりZerocopy
	- Pagefaultを起こさないためのHugepages
- 高性能通信に必要なライブラリの集合
	- Thread Manager
	- Userland Driver
	- Lock-Free Ring
	- Algorithm
	- Memory Manager
