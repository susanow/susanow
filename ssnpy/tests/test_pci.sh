#!/bin/sh

echo "[+] alloc ports and vnf"
ssnctl vnf  alloc vnf0 l2fwd1b
ssnctl port alloc eth0 pci 0000:3b:00.1
ssnctl port alloc eth1 pci 0000:5f:00.1

echo "[+] attach ports"
ssnctl vnf attachport vnf0 0 eth0
ssnctl vnf attachport vnf0 1 eth1

echo "[+] deploy and undeploy"
ssnctl vnf reset vnf0
ssnctl vnf setcoremask vnf0 0 4
ssnctl vnf deploy vnf0
ssnctl vnf undeploy vnf0

echo "[+] detach ports"
ssnctl vnf detachport vnf0 0
ssnctl vnf detachport vnf0 1

echo "[+] delete ports and vnf"
ssnctl port delete eth0
ssnctl port delete eth1
ssnctl vnf  delete vnf0


