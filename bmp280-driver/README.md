![ARM64 CI](https://github.com/salihassan706/embedded-linux-drivers/actions/workflows/embedded-ci.yml/badge.svg)
# ARM64 Embedded Linux Driver Framework

**Author:** Sohail Ali Hassan Abbasi  
**Hardware:** ARM64 (tested on Raspberry Pi 3 and QEMU)

## What This Is

A complete embedded Linux driver development and testing framework that:
- Cross-compiles Linux kernel modules for ARM64
- Boots a complete ARM64 Linux system in QEMU
- Automatically tests driver loading and operation
- Runs full CI pipeline on every git push

## Project Structure



├── module_src/          # Kernel module source code
│   ├── hello_arm64.c    # ARM64 test module
│   └── Makefile
├── rootfs_files/        # Root filesystem files
│   └── init             # System init script
├── .github/workflows/   # GitHub Actions CI
│   └── embedded-ci.yml
├── run_tests.sh         # Automated test runner
├── check_results.py     # Python test harness
└── boot.sh              # Manual QEMU boot script

## Quick Start

```bash
# Install dependencies
sudo apt install qemu-system-aarch64 gcc-aarch64-linux-gnu python3

# Build kernel (first time only - takes 40 minutes)
wget https://cdn.kernel.org/pub/linux/kernel/v6.x/linux-6.1.tar.xz
tar xf linux-6.1.tar.xz
cd linux-6.1
make ARCH=arm64 CROSS_COMPILE=aarch64-linux-gnu- defconfig
make ARCH=arm64 CROSS_COMPILE=aarch64-linux-gnu- -j$(nproc) Image
make ARCH=arm64 CROSS_COMPILE=aarch64-linux-gnu- modules_prepare
cd ..

# Build and run tests
cd module_src && make && cd ..
./run_tests.sh
```

## Test Results
[PASS] System booted successfully
[PASS] hello_arm64 module loaded successfully
[PASS] Module init message verified
[PASS] Module load confirmed by init script
All tests passed. ARM64 embedded Linux framework verified.

## Skills Demonstrated

- Linux kernel cross-compilation (ARM64)
- Kernel module development
- BusyBox rootfs construction
- QEMU system emulation
- Automated embedded Linux testing
- GitHub Actions CI/CD pipeline
