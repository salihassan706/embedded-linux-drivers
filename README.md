# Embedded Linux Driver Portfolio

**Author:** Sohail Ali Hassan Abbasi  
**Hardware:** Raspberry Pi 3 Model B+ | ARM64 | Kernel 6.12

![ARM64 CI](https://github.com/salihassan706/embedded-linux-drivers/actions/workflows/embedded-ci.yml/badge.svg)

## Projects

### 01 — Embedded Linux Drivers
Kernel drivers written and tested on Raspberry Pi 3 running kernel 6.12.
- Character device driver with mutex-protected circular buffer
- I2C device driver with Device Tree overlay
- GPIO interrupt handler with microsecond timing
- sysfs interface driver exposing sensor data to userspace

### 02 — ARM64 QEMU Framework
Complete ARM64 embedded Linux system built from source with automated CI.
- Linux 6.1 kernel cross-compiled for ARM64
- BusyBox minimal rootfs
- GitHub Actions CI — boots QEMU and verifies drivers on every push
- Python test harness with pass/fail reporting

### 03 — BMP280 Temperature Sensor Driver
Complete I2C kernel driver for Bosch BMP280 sensor on real hardware.
- Chip ID verification, factory calibration reading
- Bosch compensation formula for real temperature
- sysfs interface — cat /sys/kernel/bmp280/temperature
- Device Tree overlay for automatic boot-time binding
- Tested: 36.51°C reading on real hardware

## Skills Demonstrated
Linux kernel development, I2C/SPI drivers, Device Tree, ARM64 cross-compilation, QEMU system emulation, GitHub Actions CI, Python automation
