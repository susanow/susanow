
# Pipeline 段数による性能の変化測定

##  Traffic

```
# sudo ./app/x86_64-native-linuxapp-gcc/pktgen \
        -- \
				        -P \
								        -m "[1-4].0-1"
```
```
> enable all latency
> set all size 96   # latecy計測は96以上でないと計測されない

> start 0
> stop 0
```


