#!/bin/bash

cd busybox/_install && find . | cpio -o --format=newc > ../rootfs.img && cd -

qemu-system-x86_64 \
-m 64M \
-nographic \
-kernel ./linux/arch/x86_64/boot/bzImage \
-initrd  ./busybox/rootfs.img \
-append "root=/dev/ram rw console=ttyS0 oops=panic panic=1 kaslr" \
-smp cores=2,threads=1 \
-cpu kvm64
