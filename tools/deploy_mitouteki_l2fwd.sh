#!/bin/bash

ssnctl vnf vnf0 alloc l2fwd1b
ssnctl port pci0  alloc pci 0000:3b:00.1
ssnctl port pci1  alloc pci 0000:5e:00.1
ssnctl vnf vnf0 attachport 0 pci0
ssnctl vnf vnf0 attachport 1 pci1

ssnctl vnf vnf0 reset
ssnctl vnf vnf0 setcoremask 0 4
ssnctl vnf vnf0 deploy

