
# Roadmap

- 最終更新: 2017Q4 (10/25)
- Hiroki SHIROKURA (slank.dev[at]gmail.com)

## 最重要の未完了タスク

- **D2engine実装**:
	- 各コンポーネントの名前をつける(podのような)
		- 説明や理解が簡単になることがわかった.
		- logical graphとphysical graph?
		- policy graphとinstance graph?
	- D2管理用のエージェントを実装する
	- D2のオペレーションを列挙,設計する
	- D2のオペレーションを実装する
	- 動かす場所
		- 他プロセスで動かすか
		- greenスレッドで動かすか
	- E2を参考にする

- **NFVi実装**:
	- vport
	- agent

- **VNFを実装する**:
	- l3fwd
	- acl (5tuple)
	- dpi

- **性能計測を行う**:
	- スレッド起動
	- D2オーバヘッド
	- 仮装NIC性能


## その他の未完了タスク

以下は未踏期間中に全てやるわけではありません.
若干私の興味の範囲に偏っている場合があり, SSN計画の方針と合わないものも
あるかもしれません.

- 設計と実装のドキュメントを書く
- ドキュメントの英語対応
- D2をVMに対応する
	- プログラミングモデルはそのまま流用することができる
	- CPUホットプラグを用いて, VMにCPUをわたす
	- DPDKの``rte_eal``の中のlcore初期化部分をプログラムを
	  動かしたまま行えるようにする
	- ``rte_eal_remote_launch``を追加でできるようにする
	- SpecialThanks: おるみんちゃん, liva師
- D2をコンテナ対応する
	- まったくよくわからないが, VM対応より簡単そう?
	- コンテナ間データ共有を``rte_mempool``でする
- 複数ノード(IAサーバ)で動作可能にする
	- 移動可能オブジェクトはVNFのみである
	- 実行ファイル状態``ssn_vnf_fw.o``形式でリモートに送信
	- 受け取り側で``dlopen``か何かで動的リンク
	- クラス型にキャストしてVNFをデプロイする
	- RPCの定義にすこし近いとおもうのは私だけ？
	- thread移送というキーワードも近いかな
- pipeline対応
	- 複数段のpipelineな場合に対応する
	- 現状, pipeline段数をNFとかんがえるとこれはほぼ実現されているが
    VNFでカプセリングしたい場合があるので
- 追加の制御インターフェースを実装する
	- スレッド状態をdot言語で可視化するという話がある
	- 現在(2017Q4)はTelnet接続のVTYのみ
	- REST APIやgRPCなど
	- gRPCは対応したいと思っている
	- WebUI, RestAPIはOpenDaylightに実装されているので,参考する?
	- Lagopus2も, openconfigdなどgRPCのサンプル


## 情報収集するべき内容

- **DCA(Direct Cache Access)の勉強:**<br>
	- DCA技術を学ぶ
	- 高性能通信の上でのボトルネックになりう
	- どのような構造になっているか

- **SmartNICに関して:**<br>
	- どのようなことができるのか (性能,機能)
	- どれくらいの難易度でできるのか (値段,導入難易度)
	- DPDKコミュニティとNFVコミュニティのSmartNICに対する意見

## メモ

- 10/28現在, VNFのデプロイ追加登録は基盤ごと再コンパイルが必要

