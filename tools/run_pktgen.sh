#!/bin/sh


PATH=~/git/pktgen/pktgen

$PATH -- -m "[1-3:4-7].[0-1:0-1]" -P -f pktgen_config.txt

