# UART Sensor Stream — MPU6050 to MacBook via CP2102

**Author:** Sohail Ali Hassan Abbasi  
**Hardware:** Raspberry Pi 3 | MPU6050 | CP2102 USB-TTL | MacBook  
**License:** GPL v2

---

## What This Is

A complete sensor data streaming system. The Raspberry Pi reads live accelerometer and gyroscope data from the MPU6050 kernel driver via sysfs and streams it over UART to a MacBook using a CP2102 USB-TTL converter. A Python script on the MacBook receives and displays the data in real time with timestamps.

---

## System Architecture

```
MPU6050 sensor
    |
    |  I2C bus (SDA/SCL)
    v
mpu6050.ko kernel driver
    |
    |  sysfs /sys/kernel/mpu6050/
    v
uart_stream.sh reads sensor values
    |
    |  UART /dev/ttyAMA0 at 115200 baud
    v
CP2102 USB-TTL converter
    |
    |  USB cable
    v
MacBook uart_receiver.py
    |
    v
Live timestamped data on terminal
```

---

## Sample Output

```
[16:01:43] ACCEL:0.630,0.807,0.126 GYRO:0.038,-4.465,4.877
[16:01:44] ACCEL:0.599,0.815,0.120 GYRO:-1.374,-3.801,1.328
[16:01:45] ACCEL:0.559,0.850,0.170 GYRO:-1.068,-1.969,3.244
[16:01:51] ACCEL:0.368,0.912,0.269 GYRO:5.717,-15.061,4.580
```

Values change visibly when the sensor is tilted or moved — gravity shifts between X, Y, Z axes.

---

## Wiring

### MPU6050 to Raspberry Pi

| MPU6050 | Raspberry Pi Pin | Function    |
|---------|-----------------|-------------|
| VCC     | Pin 1           | 3.3V power  |
| GND     | Pin 6           | Ground      |
| SDA     | Pin 3 (GPIO2)   | I2C data    |
| SCL     | Pin 5 (GPIO3)   | I2C clock   |

### CP2102 to Raspberry Pi

| CP2102 | Raspberry Pi Pin  | Function       |
|--------|------------------|----------------|
| TX     | Pin 10 (GPIO15)  | UART RX        |
| RX     | Pin 8  (GPIO14)  | UART TX        |
| GND    | Pin 6            | Ground         |

CP2102 USB end connects to MacBook USB port.

---

## How to Run

### On Raspberry Pi

```bash
# Load MPU6050 driver
sudo insmod ~/kernel_modules/mpu6050.ko
echo "mpu6050 0x68" | sudo tee /sys/bus/i2c/devices/i2c-1/new_device

# Set UART baud rate
stty -F /dev/ttyAMA0 115200

# Start streaming
bash uart_stream.sh
```

### On MacBook

```bash
# Setup Python environment
python3 -m venv uart_env
source uart_env/bin/activate
pip install pyserial

# Start receiver
python3 uart_receiver.py
```

---

## Files

```
05-uart-sensor-stream/
├── uart_test.c        <- C program testing basic UART send/receive
├── uart_stream.sh     <- Shell script streaming MPU6050 data over UART
├── uart_receiver.py   <- Python script receiving data on MacBook
└── README.md          <- this file
```

---

## Technical Details

**UART configuration:** 115200 baud, 8 data bits, no parity, 1 stop bit (8N1)

**Data format:** `ACCEL:x,y,z GYRO:x,y,z\n` — comma separated values, newline terminated

**Baud rate persistence:** The Pi resets UART baud rate to 9600 after reboot. The stream script sets it to 115200 on every run.

**Sensor driver:** MPU6050 kernel driver (see `04-mpu6050-driver/`) must be loaded before running the stream script.

---

## Skills Demonstrated

- UART serial communication at 115200 baud
- Integration of kernel driver output with userspace shell scripts
- Cross-platform data streaming from ARM Linux to macOS
- Python serial communication using pyserial
- CP2102 USB-TTL hardware interface and configuration
- Real-time sensor data acquisition and display
