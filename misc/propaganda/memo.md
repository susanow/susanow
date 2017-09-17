

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

## Challenges

- Scallability
- High Performance Function Chaining
- NUMA Aware

## Memo

- サービスチェインはNSH(Network Service Header)を採用する予定である
- スレッドチューニング環境を発明し,
- そのキラーアプリとしてSFCやDPIを発表することにする


## Application

- 5-tuple ACL FW
- DPI: QoS Filter (IPv6優先NWの構築)
- DPI: Tcp80上のサービス識別, フィルタ
- L3 SW

## 今後について

- VNFリポジトリを充実させる
- DPDK CPU所有の変更
- 別プロセスNFデプロイ対応
- 別プロセスNF, 仮装NIC
- VM対応



