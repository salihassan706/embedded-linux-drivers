# SPI Loopback — Linux Kernel SPI Driver

**Author:** Sohail Ali Hassan Abbasi  
**Hardware:** Raspberry Pi 3 Model B+ | Kernel 6.12  
**License:** GPL v2

---

## What This Is

A complete Linux kernel SPI driver that performs a loopback test — transmits data over MOSI and receives it back on MISO simultaneously. Demonstrates full-duplex SPI communication, kernel SPI API usage, and sysfs interface creation.

This is a standard technique used by embedded engineers to verify SPI hardware and driver functionality before connecting real sensors.

---

## Quick Start

```bash
# Load driver and bind to SPI device
sudo insmod spi_loopback.ko
echo spi0.0 | sudo tee /sys/bus/spi/drivers/spi_loopback/bind

# Read loopback result
cat /sys/kernel/spi_loopback/loopback
# Output: HELLO FROM SPI
```

---

## How It Works

```
Raspberry Pi SPI Hardware
     |
     |-- MOSI (Pin 19 / GPIO10) ──┐
     |                            │ jumper wire
     |-- MISO (Pin 21 / GPIO9)  ──┘
     |
     v
spi_loopback.ko (this driver)
     |
     |-- 1. Configures SPI: mode 0, 500kHz, 8 bits per word
     |-- 2. On sysfs read: transmits "HELLO FROM SPI" over MOSI
     |-- 3. Simultaneously receives data back on MISO
     |-- 4. Compares sent vs received
     |-- 5. Exposes result via sysfs
               |
               v
     /sys/kernel/spi_loopback/loopback
     → HELLO FROM SPI
```

---

## SPI Protocol — Key Concepts

SPI (Serial Peripheral Interface) is a synchronous full-duplex serial protocol:

| Feature | SPI | I2C | UART |
|---------|-----|-----|------|
| Wires | 4 (MOSI, MISO, SCK, CS) | 2 (SDA, SCL) | 2 (TX, RX) |
| Speed | Up to 100MHz | Up to 3.4MHz | Up to 10Mbps |
| Devices | Multiple (one CS per device) | Multiple (addresses) | 2 only |
| Duplex | Full duplex | Half duplex | Full duplex |
| Clock | Master provides clock | Master provides clock | No clock |

**Full duplex** means data is sent and received simultaneously on every clock cycle. While master sends on MOSI, slave sends back on MISO at the same time.

**SPI Modes:**

| Mode | CPOL | CPHA | Clock idle | Data sampled |
|------|------|------|-----------|--------------|
| 0    | 0    | 0    | Low       | Rising edge  |
| 1    | 0    | 1    | Low       | Falling edge |
| 2    | 1    | 0    | High      | Falling edge |
| 3    | 1    | 1    | High      | Rising edge  |

This driver uses **Mode 0** — most common default for SPI sensors.

---

## Wiring — Loopback

Only one wire needed:

```
Pi Pin 19 (MOSI / GPIO10) ──── Pi Pin 21 (MISO / GPIO9)
```

Everything sent on MOSI is immediately received back on MISO.

### Full GPIO Header Reference for SPI

| Signal | GPIO   | Pi Pin |
|--------|--------|--------|
| MOSI   | GPIO10 | Pin 19 |
| MISO   | GPIO9  | Pin 21 |
| SCLK   | GPIO11 | Pin 23 |
| CE0    | GPIO8  | Pin 24 |
| CE1    | GPIO7  | Pin 26 |

---

## Kernel SPI API

### SPI Transfer Structure

```c
struct spi_transfer t = {
    .tx_buf = tx_data,     /* data to transmit */
    .rx_buf = rx_data,     /* buffer for received data */
    .len    = length,      /* number of bytes */
};
```

### SPI Message

```c
struct spi_message m;
spi_message_init(&m);
spi_message_add_tail(&t, &m);
spi_sync(spi, &m);         /* blocking transfer */
```

`spi_sync()` blocks until transfer completes. For non-blocking use `spi_async()`.

### SPI Configuration in Probe

```c
spi->max_speed_hz = 500000;   /* 500kHz */
spi->mode = SPI_MODE_0;        /* CPOL=0, CPHA=0 */
spi->bits_per_word = 8;        /* 8 bits per transfer */
spi_setup(spi);                /* apply configuration */
```

---

## Driver Structure

### Probe vs Remove

```
probe()  → called when device binds to driver
           allocate memory, configure hardware, create sysfs
           
remove() → called when device unbinds
           cleanup sysfs, free resources
```

### Comparison: I2C vs SPI Driver Pattern

```c
/* I2C */                          /* SPI */
struct i2c_client *client          struct spi_device *spi
devm_kzalloc(&client->dev, ...)    devm_kzalloc(&spi->dev, ...)
i2c_set_clientdata(client, data)   spi_set_drvdata(spi, data)
i2c_get_clientdata(client)         spi_get_drvdata(spi)
i2c_smbus_read_byte_data(...)      spi_sync(spi, &message)
struct i2c_driver                  struct spi_driver
module_i2c_driver(...)             module_spi_driver(...)
```

Pattern is identical — only function names change.

---

## Installation

```bash
# Enable SPI on Raspberry Pi
sudo raspi-config
# Interface Options → SPI → Enable → Reboot

# Verify SPI devices
ls /dev/spi*
# Should show: /dev/spidev0.0  /dev/spidev0.1

# Build driver
make

# Load driver
sudo insmod spi_loopback.ko

# Unbind default spidev driver
echo spi0.0 | sudo tee /sys/bus/spi/drivers/spidev/unbind

# Bind our driver
echo spi0.0 | sudo tee /sys/bus/spi/drivers/spi_loopback/bind

# Test
cat /sys/kernel/spi_loopback/loopback
# Output: HELLO FROM SPI
```

---

## Verify Driver Loaded

```bash
# Check module loaded
lsmod | grep spi_loopback

# Check probe fired
sudo dmesg | grep spi_loopback
# spi_loopback spi0.0: SPI loopback ready
# spi_loopback spi0.0: Read: cat /sys/kernel/spi_loopback/loopback

# Check sysfs file exists
ls /sys/kernel/spi_loopback/
```

---

## Files

```
06-spi-loopback-driver/
├── spi_loopback.c    <- kernel SPI driver source
├── Makefile          <- kernel module build file
└── README.md         <- this file
```

---

## What I Learned Building This

- SPI protocol — full duplex, clock modes, chip select
- Kernel SPI API — spi_transfer, spi_message, spi_sync
- SPI driver model — probe/remove with spi_device
- Driver binding — unbind spidev, bind custom driver
- spi_setup() for hardware configuration
- Difference between I2C and SPI driver patterns in kernel
- SPI loopback testing technique used in professional BSP work

---

## Next Steps

This loopback driver is a foundation. Next projects:

```
→ Connect real SPI sensor (BME280, ADXL345)
→ Replace loopback with real register reads
→ Add Device Tree overlay for automatic binding
→ Implement DMA transfers for high speed data
```

---

## References

- Linux Kernel SPI Documentation: https://www.kernel.org/doc/html/latest/spi/
- Raspberry Pi SPI Documentation: https://www.raspberrypi.com/documentation/computers/raspberry-pi.html
- SPI Protocol Specification: https://en.wikipedia.org/wiki/Serial_Peripheral_Interface
