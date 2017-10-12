
# OpenStack情報収集

## アドバイス

@uhehehe366様, Twitterより感謝いたします. 

- OpenStackコンポーネントのMagnumを使ってdockerイメージを制御
- OpenStackコンポーネントのNovaを使ってハイパーバイザがわりにDockerを利用
- OpenStackコンポーネントのNovaからKVMを制御してKVM上で動くLinuxの上でDockerを利用

- OpenStackコンポーネント群をDockerコンテナで動かす 
  (OpenStack Kollaプロジェクト) くらいかなぁ

- ちなみに蛇足に蛇足を重ねると、k8s交えるとさらに話がややこしくなります。
  DockerとOpenStackだけだと大体さっきあげ切った感じになります。　
	OpenStack + OvS + DPDKとか楽しいぞい。沼へようこそ。


## 参考文献

- OpenStack Kolla について
	- https://www.business-on-it.com/2004/20161017_kolla/

- IaaSを構成するOpenStackの主要5プロジェクト
	- http://itpro.nikkeibp.co.jp/article/COLUMN/20120518/397467/
