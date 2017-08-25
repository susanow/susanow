
# RSS情報収集


## 概要

- 82599 datasheet
- 7.1.2 Rx Queue Assgnment
- 7.1.2.8 Receive-Side Scaling
- 7LSBをRETAに渡す


## DPDKで使用する方法

```
rxmode.mqmod = ETH_MQ_RX_RSS
conf.rx_adv_conf.rss_conf.rss_key = NULL
conf.rx_adv_conf.rss_conf.rss_hf   = ETH_RSS_IP
```

これでmbuf.hash.rssにパケットのhashが入る
地道にIP srcの違うパケットを送って、どこで受けとるかをチェックしていく。


## RETA

- 128-entry Strcuture
- Index by 7LSB of hash
- Updatable during runtime
- Updating is async


## 用語

- RSSRK (RSS key)
- RETA (Rerirection Table)
- LSB (Least Significant Bit) 最下位
- MSB (Most Significatn Bit)　最上位

Related Techs
- VMDQ (VM Distrobuted Queue)
- DCB (DataCenter Bridging)


## 参考文献

- 82599datasheet http://www.intel.co.jp/content/dam/www/public/us/en/documents/datasheets/82599-10-gbe-controller-datasheet.pdf


