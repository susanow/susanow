#!/bin/bash


ssnctl vnf vnf0 undeploy
ssnctl vnf vnf0 reset
# ssnctl vnf vnf0 setcoremask 0 4
# ssnctl vnf vnf0 setcoremask 0 12
ssnctl vnf vnf0 setcoremask 0 60
ssnctl vnf vnf0 deploy

