# UART Sensor Stream — MPU6050 to MacBook via CP2102

Author: Sohail Ali Hassan Abbasi
Hardware: Raspberry Pi 3 | MPU6050 | CP2102 USB-TTL | MacBook

## What This Is

A complete sensor data streaming system. The Raspberry Pi reads
live accelerometer and gyroscope data from the MPU6050 kernel
driver via sysfs and streams it over UART to a MacBook using
a CP2102 USB-TTL converter. A Python script on the MacBook
receives and displays the data in real time with timestamps.

## System Architecture

MPU6050 sensor
    | I2C bus
    v
mpu6050.ko kernel driver
    | sysfs /sys/kernel/mpu6050/
    v
uart_stream.sh reads sensor values
    | UART /dev/ttyAMA0 at 115200 baud
    v
CP2102 USB-TTL converter
    | USB
    v
MacBook uart_receiver.py
    | displays live timestamped data

## Sample Output

[16:01:43] ACCEL:0.630,0.807,0.126 GYRO:0.038,-4.465,4.877
[16:01:44] ACCEL:0.599,0.815,0.120 GYRO:-1.374,-3.801,1.328
[16:01:45] ACCEL:0.559,0.850,0.170 GYRO:-1.068,-1.969,3.244

## Wiring

MPU6050 to Raspberry Pi:
  VCC -> Pin 1  (3.3V)
  GND -> Pin 6  (GND)
  SDA -> Pin 3  (GPIO2)
  SCL -> Pin 5  (GPIO3)

CP2102 to Raspberry Pi:
  TX  -> Pin 10 (GPIO15 UART RX)
  RX  -> Pin 8  (GPIO14 UART TX)
  GND -> Pin 6  (GND)

CP2102 USB -> MacBook USB port

## How to Run

On Raspberry Pi:
  sudo insmod ~/kernel_modules/mpu6050.ko
  echo "mpu6050 0x68" | sudo tee /sys/bus/i2c/devices/i2c-1/new_device
  bash uart_stream.sh

On MacBook:
  python3 -m venv uart_env
  source uart_env/bin/activate
  pip install pyserial
  python3 uart_receiver.py

## Files

uart_test.c       - C program to test UART communication
uart_stream.sh    - Shell script streaming sensor data over UART
uart_receiver.py  - Python script receiving data on MacBook

## Skills Demonstrated

- UART serial communication at register level
- Integration of kernel driver with userspace tools
- Cross-platform data streaming (ARM Linux to macOS)
- Python serial communication with pyserial
- CP2102 USB-TTL hardware interface
