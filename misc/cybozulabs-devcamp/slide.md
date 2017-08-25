
# ラボユース夏の開発合宿

Hiroki SHIROKURA
- slank.dev@gmail.com
- @slankdev

---
## 5tuple-ACLパケットフィルタの開発

- DPDKでl2fwd
- librte\_aclを5tuple専用にしてC++でラップ
- 性能計測 (Scalar, AVX2)
- ライブラリ部分をのぞいて100行

最初 (非バルク処理)
<p><img src="./fig1.saisyo.png" width="100%"/></p>

バルク処理(Scalar) -> 意味なし?
<p><img src="./fig2.bulk_scalar.png" width="100%"/></p>

バルク処理(AVX2) -> 性能悪くね?
<p><img src="./fig3.bulk_avx2.png" width="100%"/></p>

---
## nDPIを使ったDPIパケットフィルタ

- nDPIの入門
- pcapから読み込んでパケットフィルタするプログラム

---
## 本業の情報整理

- プレゼンスライドの整理
- 宣伝用スライドの整理
- 今後の予定整理　

---
## 今後の予定

- DPDK API C++ラップ
- スレッドチューニング部分の設計実装
- マシン購入 (2-wayマシン欲しい!!!)



