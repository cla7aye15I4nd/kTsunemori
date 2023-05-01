#!/bin/bash

cd test/hello && make && cd -

cp test/hello/hello.ko busybox/_install

chmod +x busybox/_install/*.ko

cd busybox/_install && find . | cpio -o --format=newc > ../rootfs.img && cd -