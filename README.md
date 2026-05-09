# Embedded Linux Drivers

Production-quality driver examples from kernel development course.
Each driver built, tested, and verified on real Raspberry Pi 3 hardware.

## Directory Structure
character-device/  — Character device driver with file operations
i2c-driver/        — I2C device driver with probe/remove lifecycle
gpio-interrupt/    — GPIO interrupt handler with timing measurement
sysfs-interface/   — sysfs attribute interface creation
userspace/         — User-space application example

## Building

Each driver compiles independently:

```bash
cd character-device
make

cd ../i2c-driver
make

cd ../gpio-interrupt
make

cd ../sysfs-interface
make
```

## Code Quality

✓ No debug code left in production
✓ Proper error handling on all system calls
✓ Null pointer checks where needed
✓ Correct cleanup on error paths
✓ Consistent Linux kernel coding style (scripts/Lindent)
✓ Module metadata and licensing

## Drivers

### chardev.c
Character device driver demonstrating:
- File operations (read/write)
- Mutex protection for concurrent access
- Proper error handling
- Safe user-space data transfer (copy_to/from_user)

### i2c_dummy.c
I2C device driver demonstrating:
- Device probe and remove lifecycle
- Device tree binding (of_device_id)
- Proper device initialization
- Error handling in probe path

### gpio_irq.c
GPIO interrupt handler demonstrating:
- GPIO request and direction setup
- IRQ number mapping from GPIO
- Interrupt handler with proper return codes
- Debouncing with timestamp comparison
- Proper resource cleanup

### sysfs_driver.c
sysfs interface demonstrating:
- kobject creation and management
- Attribute creation and removal
- Show/store callbacks with proper locking
- Safe attribute access from user-space

### read_sensor.c
User-space application demonstrating:
- Opening sysfs files from user-space
- Reading sensor data
- Error handling (open, read, close)
- Safe buffer management

## Hardware

All drivers tested on:
- **Device:** Raspberry Pi 3 Model B
- **Kernel:** 6.1.x (custom compiled with Buildroot)
- **Architecture:** ARM (Cortex-A53)

## Building on Real Hardware

```bash
# On Raspberry Pi 3 with kernel headers installed
make

# Insert module
sudo insmod chardev.ko

# Check if loaded
lsmod | grep chardev

# Remove module
sudo rmmod chardev
```

## Status

- ✓ chardev.c: Tested and verified on Pi 3
- ✓ i2c_dummy.c: Tested with device tree overlay
- ✓ gpio_irq.c: Tested with GPIO interrupt on Pi 3
- ✓ sysfs_driver.c: Tested with sysfs access
- ✓ read_sensor.c: User-space integration verified

## Author

Sohail Hassan

## License

GPL v2

---

**Note:** These are reference implementations from embedded Linux kernel development. Each demonstrates correct patterns for production driver code.
