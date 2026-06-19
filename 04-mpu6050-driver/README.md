# MPU6050 6-Axis IMU — Linux Kernel I2C Driver

**Author:** Sohail Ali Hassan Abbasi  
**Hardware:** Raspberry Pi 3 Model B+ | Kernel 6.12 | MPU6050 GY-521  
**License:** GPL v2

---

## What This Is

A complete Linux kernel I2C driver for the InvenSense MPU6050 6-axis IMU sensor. Reads accelerometer (X,Y,Z) and gyroscope (X,Y,Z) data via register-level I2C communication and exposes all six values through individual sysfs files. Includes a Python data logger for real-time data capture.

---

## Quick Start

```bash
# Read accelerometer (units: g)
cat /sys/kernel/mpu6050/accel_x    # e.g. 0.314
cat /sys/kernel/mpu6050/accel_y    # e.g. 0.049
cat /sys/kernel/mpu6050/accel_z    # e.g. -1.130

# Read gyroscope (units: degrees/second)
cat /sys/kernel/mpu6050/gyro_x     # e.g. 2.435
cat /sys/kernel/mpu6050/gyro_y     # e.g. 0.274
cat /sys/kernel/mpu6050/gyro_z     # e.g. 0.473

# Log data to CSV at 10Hz
python3 mpu6050_logger.py
```

---

## How It Works

```
MPU6050 Sensor (I2C address 0x68)
     |
     |  I2C bus — SDA (Pin 3) + SCL (Pin 5)
     v
Linux I2C Subsystem
     |
     v
mpu6050.ko (this driver)
     |
     |-- 1. Wakes sensor — writes 0x00 to PWR_MGMT_1 (0x6B)
     |-- 2. Reads high + low byte for each axis
     |-- 3. Combines: raw = (high << 8) | low
     |-- 4. Converts: accel = raw / 16384  (g units)
     |                gyro  = raw / 131    (degrees/second)
     |-- 5. Exposes 6 values via sysfs
               |
               v
          /sys/kernel/mpu6050/
          ├── accel_x
          ├── accel_y
          ├── accel_z
          ├── gyro_x
          ├── gyro_y
          └── gyro_z
```

---

## Register Map

| Register | Address | Description                  |
|----------|---------|------------------------------|
| PWR_MGMT_1   | 0x6B | Wake sensor (write 0x00)  |
| ACCEL_XOUT_H | 0x3B | X-axis accel high byte    |
| ACCEL_XOUT_L | 0x3C | X-axis accel low byte     |
| ACCEL_YOUT_H | 0x3D | Y-axis accel high byte    |
| ACCEL_YOUT_L | 0x3E | Y-axis accel low byte     |
| ACCEL_ZOUT_H | 0x3F | Z-axis accel high byte    |
| ACCEL_ZOUT_L | 0x40 | Z-axis accel low byte     |
| GYRO_XOUT_H  | 0x43 | X-axis gyro high byte     |
| GYRO_XOUT_L  | 0x44 | X-axis gyro low byte      |
| GYRO_YOUT_H  | 0x45 | Y-axis gyro high byte     |
| GYRO_YOUT_L  | 0x46 | Y-axis gyro low byte      |
| GYRO_ZOUT_H  | 0x47 | Z-axis gyro high byte     |
| GYRO_ZOUT_L  | 0x48 | Z-axis gyro low byte      |

Low byte is always at address = high byte register + 1.

---

## Wiring

| MPU6050 Pin | Raspberry Pi Pin | Function      |
|-------------|-----------------|---------------|
| VCC         | Pin 1           | 3.3V power    |
| GND         | Pin 6           | Ground        |
| SCL         | Pin 5 (GPIO3)   | I2C clock     |
| SDA         | Pin 3 (GPIO2)   | I2C data      |

No external pull-ups needed — Raspberry Pi has internal pull-ups on I2C lines.

---

## Data Logger

`mpu6050_logger.py` reads all 6 axes at 10Hz and saves to CSV:

```
timestamp,accel_x,accel_y,accel_z,gyro_x,gyro_y,gyro_z
2026-06-07T22:14:01,0.314,0.049,-1.130,2.435,0.274,0.473
2026-06-07T22:14:01,0.318,0.051,-1.128,2.441,0.269,0.471
```

When the sensor is flat on a surface, `accel_z` reads approximately `-1.0g` — gravity pulling straight down.

---

## Installation

```bash
# Build
make

# Load driver
sudo insmod mpu6050.ko

# Register device
echo "mpu6050 0x68" | sudo tee /sys/bus/i2c/devices/i2c-1/new_device

# Verify
sudo dmesg | grep mpu6050
cat /sys/kernel/mpu6050/accel_z    # should show ~-1.0 when flat
```

---

## Files

```
04-mpu6050-driver/
├── mpu6050.c           <- kernel driver source
├── Makefile            <- kernel module build file
├── mpu6050_logger.py   <- Python data logger (10Hz, CSV output)
├── mpu6050_data.csv    <- sample data captured from real hardware
└── README.md           <- this file
```

---

## Skills Demonstrated

- I2C register-level communication using `i2c_smbus_read_byte_data`
- 16-bit signed value reconstruction from two 8-bit registers
- Generic register pair reader — one function handles all 6 axes
- Multiple sysfs files from single kobject
- Userspace data acquisition via Python
- Real hardware testing on ARM Raspberry Pi 3
