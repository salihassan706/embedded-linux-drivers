#!/bin/bash
stty -F /dev/ttyAMA0 115200
echo "Starting sensor stream..."

while true; do
    AX=$(cat /sys/kernel/mpu6050/accel_x)
    AY=$(cat /sys/kernel/mpu6050/accel_y)
    AZ=$(cat /sys/kernel/mpu6050/accel_z)
    GX=$(cat /sys/kernel/mpu6050/gyro_x)
    GY=$(cat /sys/kernel/mpu6050/gyro_y)
    GZ=$(cat /sys/kernel/mpu6050/gyro_z)
    printf "ACCEL:$AX,$AY,$AZ GYRO:$GX,$GY,$GZ\n" > /dev/ttyAMA0
    sleep 1
done
