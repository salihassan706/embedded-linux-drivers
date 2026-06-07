#!/bin/bash

KERNEL="linux-6.1/arch/arm64/boot/Image"
ROOTFS="rootfs.img"

echo "Starting ARM64 QEMU system..."
echo "Press Ctrl+A then X to exit"
echo ""

qemu-system-aarch64 \
    -machine virt \
    -cpu cortex-a57 \
    -m 512M \
    -kernel $KERNEL \
    -drive file=$ROOTFS,format=raw \
    -append "root=/dev/vda rw console=ttyAMA0 init=/init" \
    -nographic \
    -no-reboot
