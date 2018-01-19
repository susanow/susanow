
# Susanow

[![Build Status](https://travis-ci.org/susanow/susanow.svg?branch=master)](https://travis-ci.org/susanow/susanow)

Thread based high Performance Networking Platform on DPDK.
Please see http://susanow.dpdk.ninja

## Instrallation

Install and Start
```
$ git clone http://github.com/susanow/susanow
$ cd susanow
$ export SSN_SDK=`pwd`
$ make setup
$ make && sudo make install
$ sudo systemctl start susanow
```

Stop and Uninstall
```
$ sudo systemctl stop susanow
$ cd $SSN_SDK && sudo make uninstall
```

Check NFVi is running
```
$ ssnctl nfvi show ping
ssn-ping from localhost:8888 time=0.002
ssn-ping from localhost:8888 time=0.002
...
```

## Licence and Author

MIT Licence, see LICENCE file

Hiroki SHIROKURA

 - E-mail slank.dev@gmail.com
 - Twitter @slankdev
 - Github  slankdev
 - Facebook hiroki.shirokura

This project is powered by IPA-Mitou.
