
## Depend Software

 - dpdk-16.11 (susanow/dpdk.git)
 - L-thread slankdev (susanow/lthread.git)
 - Libslankdev (slankdev/libslankdev.git)
 -

Depend Relationship (B<-A: A depends on B)
```
pure-c++11 <- libslankdev

dpdk        <- libdpdk_cpp
libslankdev <- libdpdk_cpp
libslankdev <- libvty

libslankdev      <- libsusanow
libdpdk_cpp      <- libsusanow
liblthread_dpdk  <- libsusanow
libvty           <- libsusanow
```

