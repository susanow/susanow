
# A Guide to Network Function Virtualization (NFV)

- Hiroki SHIROKURA
- slank.dev@gmail.com
- 2017.08.03

これは私なりの要約である

[Original Link (en)](https://www.fir3net.com/Networking/Concepts-and-Terminology/a-guide-to-network-function-virtualisation-nfv.html)


## Introduction

NFVは数年前からあった用語です。
ベンダーとオープンソースコミュニティがNFVの採用を推進するようになるにつれて、
新しい頭字語と流行語の波が業界全体に広がっています。
この記事では、NFVシステムを構築するために必要なさまざまなNFVコンポーネント
（標準、プラットフォーム、アクセラレーション技術など）について説明します。

## What is NFV?

まず第一に - NFVは何ですか？
NFV（ネットワークファンクション仮想化）は、現在、物理ハードウェア
（ファイアウォール、ルータ、ロードバランサなど）上で実行されている
ネットワーク機能を標準のコモディティハードウェア上で仮想化する概念です。

## Advantages

NFVはOPEX（運用支出）とCAPEX（資本支出）の両面で数多くの利点をもたらします。
彼らは、

- 購入 <br>
  さまざまなベンダーからのさまざまなネットワークハードウェアラインの購入を
	無効にすることができるため、購入ラインを削減し、簡素化することができます。
	代わりに、標準的なx86サーバーベンダーを使ってVNF（仮想ネットワーク機能）
	をホストすることができます。
- スケーリング<br>
  VNFをすぐに構築できるため、垂直または水平スケーリングの柔軟性が向上します。
- プロビジョニング<br>
  物理サーバを購入してケーブル接続する時間と比較して、プロビジョニング時間が
	大幅に短縮されます。
- 電源/冷却<br>
  複数のVNFを1台のサーバーで実行できるため,冷却と電力消費が大幅に削減されます。
- ライセンス<br>
  場合によっては、VNFのライセンスは、物理的なものと比較して安くなります。
- ハードウェアEOL<br>
  ハードウェア からソフトウェアを切り離すため、
	物理的なネットワークハードウェアは必要ありません。
	つまり、ベンダーのさまざまなEOL /製品ライフサイクルと、
	ハードウェアのリフレッシュに関連するコストのかかるプロジェクトには
	結びつかないということです。

## Trameworks and Standards

ETSI NFV ISG
欧州電気通信標準化機構 （ ETSI ）は、電気通信業界における独立した非営利の
標準化組織です。2012年11月、NFVの必要基準を開発するとともに、NFVの実装と
テストの経験を共有するために、NFV Industry Specification Group（ISG）が
設立されました。 [1]

ETSI NFV Group Specification（GS）
NFV Architectural FrameworkとNFV Management and Orchestrationという2つの
主要なETSIがあります。

NFVの建築フレームワーク

ETSI NFV ISGアーキテクチャフレームワーク（ETSI GS NFV 002）は、
ネットワーク機能の仮想化に必要なさまざまな要素を定義することによって、
NFVを標準化します。 このフレームワークには、 高レベルNFVフレームワークと
参照アーキテクチャフレームワークの 2つの主要セクションが含まれています 。

高レベルのNFVフレームワーク - このフレームワークは、物理インフラストラクチャと
仮想インフラストラクチャにおけるネットワーク機能の実装を高いレベルで
記述しています。 これは、3つのNFV作業ドメインに基づいています。

- VNF（仮想ネットワーク機能）<br>
  NFVI上で実行できるネットワーク機能のソフトウェア実装。

- NFVI（NFV Infrastructure）<br>
  計算、ストレージ、ネットワーキングなどの VNFの ホストと接続に使用される
  一連のリソース（物理または仮想）。

- NFV管理とオーケストレーション（MANO）<br>
  VNFの プロビジョニングに必要な管理とオーケストレーション、
  ライフサイクル管理をカバーします。


## Modeling
## SDN and NFV
## Virtual Switches
## Optimization
## Jrojects
## References



