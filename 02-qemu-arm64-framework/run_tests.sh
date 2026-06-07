#!/bin/bash

KERNEL="linux-6.1/arch/arm64/boot/Image"
ROOTFS="rootfs.img"
LOG="/tmp/qemu_output.log"

echo "Starting ARM64 QEMU test..."

# Remove old log
rm -f $LOG

# Boot QEMU in background writing to log file
qemu-system-aarch64 \
    -machine virt \
    -cpu cortex-a57 \
    -m 512M \
    -kernel $KERNEL \
    -drive file=$ROOTFS,format=raw \
    -append "root=/dev/vda rw console=ttyAMA0 init=/init" \
    -display none \
    -serial file:$LOG \
    -no-reboot &

QEMU_PID=$!

# Wait for boot
echo "Waiting for system to boot..."
sleep 20

# Kill QEMU
kill $QEMU_PID 2>/dev/null

echo "Checking results..."
python3 check_results.py $LOG
