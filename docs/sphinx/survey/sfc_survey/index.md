
# Service Chaining

NFVサービスチェインとよばれることもある.
Routr, Firewall, LoadBarancerと言ったNFを連係させ, 適切な順番でパケットを
やりとりせること.
[IETF-SFC-WG ]で標準化が行われている.

「例えば、最初はルーターとファイアウォールくらいでサービスを始めたが、
やはりお客さんの要望で『DPIが欲しいよね』となってきたとき、ソフトウェアを
追加してつなげば、DPIオプションみたいなものがすぐに作れると思っている」

サービスチェイニングとは、個々のユーザーや用途などに応じて、
必要なネットワーク機能をつないで提供することで、NFVの主要なユースケース
の1つとされている。

## 特徴

- サービス機能の種別や提供する順序を示す「タグ」を用いたパケット転送
- SFFにおける経路テーブルのエントリー数は、制御するフローの数に係わらず、
  サービス機能の種別と順序の組み合わせの数だけで済む
- SDN/NFVを実装するためのプロトコル
- 利点
- 欠点


## 必要性

- 簡単にサービス追加/消去を設定できるようにしたい
- 構成変更に対してTopoを変えたくない
- ポリシーベースで適用するサービスを柔軟に構成したい


## 用語

Service Function <br>
Router, Firewall, IDS, NATなど　

Service Function Chain <br>
SFの順序付き集合. (ex. FW:DPI:NAT44)

Service Function Path <br>
Service Function Chainから具体的に決定したService Functionの経路.
(ex. DC1FW1:DC1DPI1:DC1NAT44)

Classification, Classifier <br>
定義されたポリシーとパケットの情報からService Function Pathを決定する行為.
サービスチェインの入り口に置かれ、フローの識別、および、フローに適用する
サービス（サービス機能の種別と順序）の決定を行う.
また、決定されたサービスを識別するタグをパケットに付与します。

Service Functoin Forwarder (SFF)<br>
Service Function Pathに従い, パケットをService Functionに転送する&受け取る.
ネットワーク上に置かれ、タグを見てパケットを適切なサービス機能に転送します.

SFC Proxy <br>
SFCに対応していない機器のためにパケットからSFCヘッダをとったり、つけたりする.
SFCのタグに対応していないサービス機能とSFFの間に置かれ、
タグの取り外しと再付与を行います

Controller <br>
ClassifierとSFFの管理およびテーブルの管理を行います

## Encapsulationフォーマット

- Network Service Header (NSH)
- Servie Chain Header (SCH)

## NSH (Network Service Header)

SFC用のプロトコル. IPv4,IPv6,Ethernetを転送可能. L4に依存しない.

```
  0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
 +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 |                         Base Header                           |
 +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 |                     Service Path Header                       |
 +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 |                                                               |
 ~                       Context Headers                         ~
 |                                                               |
 +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
                 Figure1: Network Service Header


  0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
 +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 |Ver|O|C|R|R|R|R|R|R|   Length  |    MD Type    | Next Protocol |
 +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
                   Figure2: NSH Base Header


    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |Ver|O|C|R|R|R|R|R|R|   Length  |  MD Type = 1  | Next Protocol |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |          Service Path ID                      | Service Index |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |                Mandatory Context Header                       |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |                Mandatory Context Header                       |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |                Mandatory Context Header                       |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |                Mandatory Context Header                       |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |                                                               |
   ~           Optional Variable Length Context Headers            ~
   |                                                               |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
               Figure 1: Network Service Header - MD Type 1


    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |D|F|Res|    Source Node ID     |    Source Interface ID        |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |    Reserved   |               Tenant ID                       |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   | Destination Class / Reserved  |        Source Class           |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |A|            ServiceTag / Opaque Service Class                |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
                    Figure 2: NSH DC Context Allocation
```



## References

- https://www.slideshare.net/OPNFV/opnfv-service-function-chaining
- RFC7665 Service Function Chaining (SFC) Architecture



