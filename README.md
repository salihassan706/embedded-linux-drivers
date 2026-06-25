# Embedded Linux Developer — Kernel Drivers | ARM64 | I2C | SPI | UART

**Author:** Sohail Ali Hassan Abbasi  
**Hardware:** Raspberry Pi 3 Model B+ | ARM64 | Kernel 6.12  
**Contact:** github.com/salihassan706

![ARM64 CI](https://github.com/salihassan706/embedded-linux-drivers/actions/workflows/embedded-ci.yml/badge.svg)

---

## About

I am an RF Engineer transitioning to embedded Linux development. This portfolio documents real kernel drivers written in C, tested on physical ARM hardware, with automated CI. Every project here was built from scratch — reading datasheets, writing drivers, debugging hardware failures, and documenting the process professionally.

---

## Projects

### 01 — Embedded Linux Drivers
Foundational kernel drivers written and tested on Raspberry Pi 3 running kernel 6.12.

- **Character device driver** — register_chrdev, file_operations, copy_to_user/copy_from_user
- **I2C dummy driver** — i2c_driver probe/remove lifecycle, Device Tree overlay
- **GPIO interrupt handler** — request_irq, IRQF_TRIGGER_RISING, interrupt counting via sysfs
- **sysfs interface driver** — kobject API, kobj_attribute, userspace data exposure

---

### 02 — ARM64 QEMU Framework
Complete ARM64 embedded Linux system built from source with automated CI pipeline.

- Linux 6.1 kernel cross-compiled for ARM64 using aarch64-linux-gnu-gcc
- BusyBox minimal rootfs built from source
- GitHub Actions CI — boots full ARM64 system in QEMU on every push
- Python test harness with automated pass/fail reporting
- Proves drivers load and operate correctly without physical hardware

---

### 03 — BMP280 Temperature Sensor Driver
Complete I2C kernel driver for Bosch BMP280 temperature sensor on real hardware.

- Register-level I2C communication — chip ID verification (0xD0 → 0x58)
- Factory calibration data reading (T1, T2, T3 registers)
- Bosch compensation formula — raw ADC to real temperature in 0.01°C units
- sysfs interface — `cat /sys/kernel/bmp280/temperature` → `36.51`
- Device Tree overlay for automatic boot-time driver binding
- Includes hardware debugging log — ESD damage diagnosis and root cause analysis

---

### 04 — MPU6050 6-Axis IMU Driver
Complete I2C kernel driver for InvenSense MPU6050 accelerometer and gyroscope.

- Wake sensor via PWR_MGMT_1 register, 16-bit signed value reconstruction
- Generic register pair reader — one function handles all 6 axes
- 6 individual sysfs files: accel_x, accel_y, accel_z, gyro_x, gyro_y, gyro_z
- Hardware interrupt registration via request_irq() on GPIO27 for motion detection
- Python data logger capturing 10Hz CSV data from real hardware
- Real hardware output: accel_z = -1.13g (gravity), gyro showing live rotation

---

### 05 — UART Sensor Stream
Real-time sensor data streaming from Raspberry Pi to MacBook via CP2102 USB-TTL.

- Disabled Bluetooth to reclaim PL011 UART (/dev/ttyAMA0) on Raspberry Pi 3
- CP2102 USB-TTL converter — TX/RX crossover wiring to Pi GPIO14/GPIO15
- C program (uart_test.c) using termios for 115200 8N1 serial configuration
- Shell script reads MPU6050 sysfs and streams formatted data over UART
- Python receiver (pyserial) on MacBook displays live timestamped sensor data
- Demonstrated full pipeline: kernel driver → sysfs → UART → USB → macOS

---

### 06 — SPI Loopback Driver
Linux kernel SPI driver demonstrating full-duplex communication and kernel SPI API.

- Full-duplex SPI transfer using spi_sync() and spi_transfer struct
- Configures SPI Mode 0 at 500kHz, 8 bits per word via spi_setup()
- MOSI connected to MISO — standard loopback test technique for BSP work
- sysfs interface — `cat /sys/kernel/spi_loopback/loopback` → `HELLO FROM SPI`
- Foundation for real SPI sensor drivers (BME280, ADXL345)

---

## Hardware

```
Raspberry Pi 3 Model B+    ARM Cortex-A53, 1GB RAM, kernel 6.12
BMP280 sensor module       Bosch temperature sensor (I2C 0x76)
MPU6050 GY-521 module      InvenSense 6-axis IMU (I2C 0x68)
CP2102 USB-TTL converter   Silicon Labs USB-to-UART bridge
Breadboards + jumper wires
```

---

## Skills Demonstrated

```
Kernel development    Linux kernel modules, probe/remove lifecycle, devm_kzalloc
I2C protocol          i2c_smbus_read_byte_data, register-level communication
SPI protocol          spi_sync, spi_transfer, full-duplex transfers, SPI modes
UART protocol         termios, 115200 8N1, CP2102 USB-TTL hardware
GPIO interrupts       request_irq, IRQF_TRIGGER_RISING, atomic counters
sysfs interface       kobject API, kobj_attribute, userspace data exposure
Device Tree           DTS overlay authoring, dtc compilation, boot-time binding
ARM64                 Cross-compilation, QEMU system emulation
CI/CD                 GitHub Actions, automated boot and driver verification
Python                pyserial, CSV data logging, test automation
Hardware debugging    i2cdetect, i2cget, dmesg, ESD damage diagnosis
```

---

## Repository Structure

```
embedded-linux-drivers/
├── 01-embedded-linux-drivers/   ← foundational kernel drivers
├── 02-qemu-arm64-framework/     ← ARM64 QEMU + CI pipeline
├── 03-bmp280-driver/            ← temperature sensor, real hardware
├── 04-mpu6050-driver/           ← 6-axis IMU, interrupt, data logger
├── 05-uart-sensor-stream/       ← UART streaming pipeline
├── 06-spi-loopback-driver/      ← SPI full-duplex driver
└── README.md                    ← this file
```

---

### 07 — IoT MQTT Sensor Dashboard
Complete IoT pipeline streaming live sensor data from Raspberry Pi to MacBook over WiFi.

- MPU6050 kernel driver reads accelerometer and gyroscope via I2C
- Python publisher reads sysfs and publishes JSON to Mosquitto MQTT broker
- MacBook Python subscriber displays live formatted data with timestamps
- Complete pipeline: kernel driver → sysfs → MQTT → WiFi → dashboard
- Real hardware proof — screenshots of live data streaming included
