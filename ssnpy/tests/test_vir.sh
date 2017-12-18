#!/bin/sh

echo "[+] alloc ports and vnf"
ssnctl vnf  alloc vnf0 l2fwd1b
ssnctl vnf  alloc vnf1 l2fwd1b
ssnctl port alloc tap0 tap tap0
ssnctl port alloc tap1 tap tap1
ssnctl port alloc vir0 virt
ssnctl port alloc vir1 virt

echo "[+] alloc ppp"
ssnctl ppp alloc ppp0 vir0 vir1

echo "[+] attach ports"
ssnctl vnf attachport vnf0 0 tap0
ssnctl vnf attachport vnf0 1 vir0
ssnctl vnf attachport vnf1 0 vir1
ssnctl vnf attachport vnf1 1 tap1

echo "[+] deploy and undeploy"
ssnctl vnf reset vnf0
ssnctl vnf reset vnf1

ssnctl vnf setcoremask vnf0 0 4
ssnctl vnf setcoremask vnf1 0 8
ssnctl vnf deploy vnf0
ssnctl vnf deploy vnf1
ssnctl vnf undeploy vnf0
ssnctl vnf undeploy vnf1

echo "[+] detach ports"
ssnctl vnf detachport vnf0 0
ssnctl vnf detachport vnf0 1
ssnctl vnf detachport vnf1 0
ssnctl vnf detachport vnf1 1

echo "[+] delete ppp"
ssnctl ppp delete ppp0

echo "[+] delete ports and vnf"
ssnctl port delete tap0
ssnctl port delete tap1
ssnctl port delete vir0
ssnctl port delete vir1
ssnctl vnf  delete vnf0
ssnctl vnf  delete vnf1


