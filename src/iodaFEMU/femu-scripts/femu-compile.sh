#!/bin/bash

NRCPUS="$(cat /proc/cpuinfo | grep "vendor_id" | wc -l)"

make clean

../configure --enable-kvm --target-list=x86_64-softmmu --disable-werror --extra-cflags=-w  --python=/usr/bin/python2

make -j $NRCPUS


