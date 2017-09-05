
# NFV Whitepaper

ETSIのNFV Whitepaperようやく

- Hiroki SHIROKURA
- slank.dev@gmail.com
- 2017.08.05

### NFVの概要
- 設備の統合によるCAPEX削減
- 迅速なデプロイや機能拡張によりネットワークの成熟サイクルの加速化
- ネットワークアプライアンスのマルチバージョン,マルチテナントで可用性向上
- 幅広いエコシステムを可能にし, 開放性を促進

### 現状の技術課題
- 高性能仮想化ネットワークアプライアンスの実現
- 専用ハードとの共存を実現しながらNFVへの効率的な移動パスの構築
- 多くのアプライアンスの管理,統合を保護しつつ,セキュリティの確保
- 全ての機能の自動化
- SW/HWの障害への復元力の確保
- 異ベンダアプライアンスの統合

### SDNとNVFの関係
- NFVはSDNを利用せずとも構築できる
- NFVの実現によりSDNをサポートすることができる

### ユースケース
- Switching: BNG, CG-NAT, Router
- Mobile NW Nodes: HLR/HSS, MME, SGSN, GGSN/PDN-GW, RNC, NodeB, eNodeB
- Tunnelling GW: IPSec/SSL-VPN-GW
- Traffic analysis: DPI, QoE測定
- Service Assurance: SLA monitoring, Test and Diagnostics
- NGN Signalling: SBCs, IMS
- Converged and Network-wide Functions: AAA Server, PolicyCtl
- ApplicationLevel Optimization: CDNs, Cache server, LB, App qaccelerators
- Security FUnctions: FW, VirusScanner, IDS, Spam protection

### 今流行っているNFVのための研究テーマ
- A software-based DPI, provideing adbanced traffic analysis and
  multi-dimensional reporting and showing the possibility
  of makeing off-the-shelf HW work at actual line-rates
	ソフトDPIはNW広範囲に展開可能.
	分析機能, 配置, 更新, テスト, ワークロード変更に合わせて調節可能
- IP node implementations, CG-NAT, BRAS
- 専用HWの必要なサービスの仮想化: FW, IPS, IDS, WANなど
- CDN: 対応サービスを動的に変更するCDN
- Mobile Core Netoworkの仮想化 (ex. M2M)
- Co-ordinated implementation of Cloud & NW for Enterprises.
- Hybrid-fibre-DSL-nodes, are located deep in the ext-NW in Streen-Cabines

### NFVの利点
- コストダウン
	- 設備コスト削減
	- 消費電力削減
- オペレーションサイクルを最小限にし,市場投入までの時間を短縮
- 同一インフラ上で本番,テスト,開発が可能
- ソフトウェア操作による迅速なサービス拡大/縮小
- エコシステムの利用 (OSSなどの活用)
- NW構成やトポロジの最適化
- マルチテナントによるリソース共有
- 標準サーバ管理技術が活用可能
- 高度な統一性
	- オーケストレーション
	- アプリ固有の専用HWの必要性の排除
	- 計画,プロビジョニングのための設備の多様化
	- 自動再構成と移動による障害修復オプション
	- 簡単な復帰による動的なサービスアップグレード(不停止)

### 変化する通信業界の景観
- 高速マルチコアCPUを搭載した高性能なパケット処理が可能
- クラウドインフラは、リソースの可用性と使用を向上させる方法を提供
- 管理,制御APIのオープン化
- 業界標準の大容量サーバ

### NFVの課題
- Portability/Interoperability
- Performance Trade-Off
- Migration & co-existence of legacy & compatibility with existing platforms
- Manage & Orchestration
- Automation
- Security & Resilience (復元力)
- Network Stability
- Simplicity
- Integration


