#!/bin/sh

docker run --rm -it \
	-v `pwd`:/tmp \
	-v /var/run:/var/run \
	-v /dev/hugepages:/dev/hugepages \
	slankdev/ubuntu:16.04 \
	/tmp/a.out
