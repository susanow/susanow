# ssnpy: Susanoy Python Module

Control-susanow-NFV's API for python3

install/uninstall
```
$ sudo make install
$ sudo make uninstall
```

## Examples

write soon
```
import susanow

nfvi = susanow.nfvi()
susanow.nfvi().delete_vnf(iname)
susanow.nfvi().delete_port(iname)
susanow.nfvi().delete_ppp(iname)

vnf = nfvi.alloc_vnf(iname, cname)
vnf = nfvi.get_vnf(vname)
vnf.show()
vnf.attach_port(pid, port)
vnf.detach_port(pid)
vnf.reset()
vnf.set_coremask(bid, mask)
vnf.deploy()
vnf.undeploy()

n = vnf.name()
p = vnf.n_port()
b = vnf.n_block()
r = vnf.running()
c = vnf.coremask()

port = nfvi.alloc_port_pci(iname, pciaddr)
port = nfvi.get_port(pname)
port.show()
nrxq = port.n_rxq()
ntxq = port.n_txq()
nrxa = port.n_rxa()
ntxa = port.n_txa()
atc  = port.attach()
rate = port.rxrate()
perf = port.perfred()

ppps = nfvi.list_ppps()
ppp = nfvi.get_ppp(pname)
ppp = nfvi.alloc_ppp(iname, l_port, r_port)
ppp.show()
```


ssnctl
```
ssnctl nfvi stat
ssnctl nfvi ping
ssnctl nfvi nic

ssnctl list
ssnctl stat        <name>
ssnctl alloc       <name> <cname>
ssnctl delete      <name>
ssnctl attachport  <name> <pid> <pname>
ssnctl detachport  <name> <pid>
ssnctl deploy      <name>
ssnctl undeploy    <name>
ssnctl reset       <name>
ssnctl setcoremask <name> <bid> <coremask>

ssnctl list
ssnctl stat   <name>
ssnctl alloc  <name> pci <0000:01:00.0>
ssnctl alloc  <name> tap <tap0>
ssnctl alloc  <name> virt
ssnctl delete <name>

ssnctl list
ssnctl stat   <name>
ssnctl alloc  <name> <left> <right>
ssnctl delete <name>
```

