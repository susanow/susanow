
# 仮想NICバックエンド情報収集

- Hiroki SHIROKURA (slank.dev[at]gmail.com)
- 2017.10.17

## 仮想NICのパターン

- e1000エミュレーション
- virtio-net
- vhost-net
- vhost-user

## 仮想NICのボトルネック

- メモリコピー
- コンテキストスイッチ
	- VMX non-root,root切り替え命令 (VM Entry/Exit)
	- OS(Guest, Host)のコンテキストスイッチ


