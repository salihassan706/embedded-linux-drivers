#!/bin/bash

KERNEL="linux-6.1/arch/arm64/boot/Image"
ROOTFS="rootfs.img"

qemu-system-aarch64 \
    -machine virt \
    -cpu cortex-a57 \
    -m 512M \
    -kernel $KERNEL \
    -drive file=$ROOTFS,format=raw \
    -append "root=/dev/vda rw console=ttyAMA0 init=/init" \
    -nographic \
    -no-reboot \
    -chardev socket,id=serial0,path=/tmp/qemu-serial.sock,server=on,wait=off \
    -serial chardev:serial0
