# MPU6050 6-Axis IMU Linux Kernel I2C Driver

Author: Sohail Ali Hassan Abbasi
Hardware: Raspberry Pi 3 Model B+, Kernel 6.12, MPU6050 GY-521
License: GPL v2

## What This Is

A complete Linux kernel I2C driver for the InvenSense MPU6050 6-axis IMU sensor. Reads accelerometer and gyroscope data via register-level I2C and exposes all six values through sysfs.

## Quick Start

Read accelerometer:
  cat /sys/kernel/mpu6050/accel_x
  cat /sys/kernel/mpu6050/accel_y
  cat /sys/kernel/mpu6050/accel_z

Read gyroscope:
  cat /sys/kernel/mpu6050/gyro_x
  cat /sys/kernel/mpu6050/gyro_y
  cat /sys/kernel/mpu6050/gyro_z

Log data to CSV:
  python3 mpu6050_logger.py

## How It Works

1. Wakes sensor by writing 0x00 to PWR_MGMT_1 register 0x6B
2. Reads high and low byte registers for each axis
3. Combines: raw = (high << 8) | low
4. Converts: accel = raw / 16384 (g units)
           gyro  = raw / 131   (degrees per second)
5. Exposes via 6 sysfs files under /sys/kernel/mpu6050/

## Register Map

PWR_MGMT_1  0x6B  Wake sensor
ACCEL_XOUT  0x3B  X-axis accelerometer high byte
ACCEL_YOUT  0x3D  Y-axis accelerometer high byte
ACCEL_ZOUT  0x3F  Z-axis accelerometer high byte
GYRO_XOUT   0x43  X-axis gyroscope high byte
GYRO_YOUT   0x45  Y-axis gyroscope high byte
GYRO_ZOUT   0x47  Z-axis gyroscope high byte

Low byte is always register + 1.

## Wiring

VCC  -> Pi Pin 1  (3.3V)
GND  -> Pi Pin 6  (GND)
SCL  -> Pi Pin 5  (GPIO3)
SDA  -> Pi Pin 3  (GPIO2)

## Data Logger

mpu6050_logger.py reads all 6 axes at 10Hz and saves to CSV.
Sample output:
  timestamp, accel_x, accel_y, accel_z, gyro_x, gyro_y, gyro_z
  2026-06-07T22:14:01, 0.314, 0.049, -1.130, 2.435, 0.274, 0.473

## Installation

make
sudo insmod mpu6050.ko
echo "mpu6050 0x68" | sudo tee /sys/bus/i2c/devices/i2c-1/new_device
cat /sys/kernel/mpu6050/accel_z

When flat on a surface, accel_z should read approximately -1.0g.

## Files

mpu6050.c          - kernel driver source
Makefile           - build file
mpu6050_logger.py  - Python data logger
mpu6050_data.csv   - sample data capture
README.md          - this file

## Skills Demonstrated

- I2C register-level communication
- 16-bit signed value reconstruction from two 8-bit registers
- Multiple sysfs files from single kobject
- Userspace data acquisition via Python
- CSV data logging for analysis
