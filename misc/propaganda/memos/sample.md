% NFVにおける最適なシステム構成の方式の研究
% \underline{Hiroki SHIROKURA}, Yasuhiro OHARA, Atsushi KANAI
% 2017/09/16


# Summary

- 目的: **より**高性能なNVF環境の構築
- 目標: KVM上のVNFを40GbEで稼働させる
- 内容:
	- OvS構成を多数検証し, 評価
	- 仮想化オーバヘッドの把握, その迂回
- 実装: 高速PCルータKamueeをvSWとして拡張

![検証するシステム](img/system.png){width=150}

# 背景: NFV (Network Functions Virtualization)

- 要因1: 汎用PCの高性能化,低価格化
- 要因2: DPDKがソフトウェア高性能通信を可能に
- 利点1: 迅速な{サービス拡大/縮小, デプロイ, 拡張}
- 利点2: CAPEX, OPEX低下
- Function Chaining
	- NF(NetworkFunction)を数珠繋ぎ,サービス柔軟化
	- 柔軟なサービスの構成変更 (ex. Router → Router+IDS)

![NFV](img/sfc.png)

# 背景: DPDK (Data Plane Development Kit)

- Numa awared Memory management system
- Network Stack bypassing for High speed Packet I/O
- CPU Assignment for bypass context switch
- Data structure & Algorithm for packet-processing
- Kamuee achieves 145Gbps with 128B packet, BGP-FULLROUTE(520K)

![dpdk](img/dpdk.png){width=250}

# 問題: VM Overhead...?

- It's OK to achieve 40GbE pkt-fwd on PC
- It's not OK to achieve that on \underline{**VMs on PC**}...
- Everyone can't explain what is VM-Overhead.... :(

![Open vSwitch Architecture](img/vm_overhead.png)


# System Architecture & Evaluation Design

- HV, vSwitch : {KVM}, {SR-IOV,OvS,OvS-dpdk}
- vNICバックエンド
	- e1000, virtio: device-full-emulation
	- vhost-net: share-memを用い,主な処理はkern-thread
	- vhost-user: userlandでshare-mem (server)
	- vhost-user-client: userlandでshare-mem (client)
- 評価: 帯域, 遅延, フレキシビリティ

![NFV構成パターン](img/fig2.png)

# Porting vhost-user on Kamuee

- 左: 既存のKamueeの構造
- 右: 本研究で拡張するKamueeの構造
	- vhost-userというvNICバックエンドを実装
	- KVM有効のVMをKamueeにつなぎ40GbE Function Chainをする

![高速PCルータKamuee](img/kamuee-vhost.png)

# Progress

完了項目

- OvS-dpdkの環境構築,性能計測(10GbE)
- vNICバックエンドの調査,まとめ
- vhost-userの実装のための資料調達
- Kamuee vhost-user対応のためのPoC vSwitch実装

今後の作業

- OvS-dpdkの性能計測(40GbE)
- SR-IOVの環境構築,性能計測(10GbE,40GbE)
- その他chainパターンの環境構築,性能計測
- Kamuee vhost-user実装
- Kamuee vhost-user性能計測(40GbE)



