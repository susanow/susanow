
## DPDKとNFVの現状

- NFVに以降できない理由は良質なVNFがすくないこと
	- ほんとは世界中でみんなでどんどんつくるべき
	- 開発者はチューニング大好き
	- 優秀な開発者はベアメタルで高速PCルータを開発する時代　
	- 作環境にカリカリコードレベルでチューニング
	- 枯れたVNFがない -> 最適化しすぎるから
	- 最適化フェーズは分割しましょう.
	- 機能,安定性にかんしてしっかりとした実装を世界ですすめるべき
	- これはラピッドプロトタイプの話ではない
	- 早急に枯れさせるためにはこのようにしないと, という話


## NFVについて

重要キーワード
- ネットワークスライス
- クラウド
- ネイティブ設計原則
- エンドツーエンドサービス管理
- エッジコンピューティング
- 無線アクセスネットワーク
- マルチサイド/ドメインサービス
- NFVライセンス管理
- セキュリティ
- 信頼性
- スケーラビリティ
- マルチベンダで防御できるようにすることが大事


## Challenges

- Scallability
- High Performance Function Chaining
- NUMA Aware


## Memo

- サービスチェインはNSH(Network Service Header)を採用する予定である
- スレッドチューニング環境を発明し,そのキラーアプリを発表することにする

- 進化するハードウェアに合わせてソフトウェアを書き直しなくない
- 基本構造が同じなら自由に最適化を自動にしてほしい
- ex. core=4 -> core=10

- 話のながれとして, どういうことをしたか
- 何に頑張ってそれをしたか (challengeはなにか)

- VNFの制御インターフェースは専用のプロトコルをUnix Socketで実装する？
- 別プロセスでVNFをデプロイするほうほうを考える
- Dockerと連携することを考える.

## Application

- 5-tuple ACL FW
- DPI: QoS Filter (IPv6優先NWの構築)
- DPI: Tcp80上のサービス識別, フィルタ
- L3 SW
- NetFlow Node (2portでNetFlowの取得だけ行う)
- 無停止な動的アップグレード
- SSLの部分的複合による脅威解析
- Security
	- FW
	- DDoS Mitigation
	- Sandbox
	- SSL 一部Decription 脅威解析
	- Dynamic ALCs Box
- 4K/5K Streaming Application
- 悪意のあるトリフィックの検知と遮断
- 次世代エンドポイントセキュリティ

## 可視化方法について

- vnfクラスからスレッド配置とポート状態をdot言語でエクスポートできるように
- スレッド遅延やパケット処理レートをvnfクラスからとれるように
- それをRestAPIでとってこれるように
- GUIクライアントなり, ブラウザなりで値をもらって1secごとに可視化

## 今後について

- VNFリポジトリを充実させる
- DPDK CPU所有の変更
- 別プロセスNFデプロイ対応
- 別プロセスNF, 仮装NIC
- VM対応
- VNFのperfをどうやって取るかを考える
- 宣伝資料に以下を追加
	- やりたいこと
	- 突き詰めたこと
	- なぜそれが可能になったか
	- どんな巨人の肩に乗っているか
	- どんな巨人になれるのか


# 複数SSN Nodeを活用する場合についての検討

SSNによって1server上での最適化は可能になったが、複数のサーバ上で動作させる
場合については検討していない.
本資料ではそのような場合について対応するための意見をまとめておくものである.

## Memo

- NUMA, 別Hostは似たような構造としてかんがえることはできないだろうか
  Slow Link(NUMA), Fast Link(別Host)みたいに


## NFVに関連するOSS

OpenDaylight
- not only OpenFlow Controller
- Supports: BGP, OpenFlow, NETCONF
- Controllerとしては世界でもっとも知名度が高い?
- 商用サポートあり

Open vSwitch (OvS)
- OpenFlow vSwitch developed as OSS
- Linuxのbridgeと互換性あり
- DPDK利用可能

OvS-DPDK
- パケット処理をDPDKによってアクセラレーション
- VM上, HV上の両方でDPDKを使うことが高性能化の条件
- ただ使うだけでなく, VMの仮装NICの種類によって性能が変化する
- OVS用にいくつかのCPUを使用する
- {sum of vCPU} > {num of cores}になったら, vm\_entry, vm\_exitの数が上昇?
- DPDKのCPUpinningの効果が低減
- VM上で動くVNFが一般的なDPDK VNFならまとめて管理をできる


```
コンピュータリソース100%10G
A33%10Gbps B33%05Gbps C33%20Gbps
A33%10Gbps B66%10Gbps C16%10Gbps
とする、アルゴリズムを開発する
```
