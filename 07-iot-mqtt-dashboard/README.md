# IoT Sensor Dashboard — MPU6050 to MacBook via MQTT

**Author:** Sohail Ali Hassan Abbasi  
**Hardware:** Raspberry Pi 3 | MPU6050 | MacBook  
**Stack:** Linux Kernel Driver | Python | MQTT | Mosquitto  
**License:** GPL v2

---

## What This Is

A complete IoT sensor pipeline that reads live accelerometer and gyroscope data from a Linux kernel driver and streams it over WiFi to a remote dashboard using the MQTT protocol. This demonstrates a real-world IoT architecture used in industrial sensor monitoring, smart home systems, and remote telemetry applications.

---

## System Architecture

```
MPU6050 Sensor (I2C address 0x68)
     |
     |  I2C bus
     v
mpu6050.ko — Linux kernel driver
     |
     |  sysfs interface
     |  /sys/kernel/mpu6050/accel_x
     |  /sys/kernel/mpu6050/accel_y
     |  /sys/kernel/mpu6050/accel_z
     |  /sys/kernel/mpu6050/gyro_x
     |  /sys/kernel/mpu6050/gyro_y
     |  /sys/kernel/mpu6050/gyro_z
     v
mpu6050_publisher.py
     |
     |  MQTT publish (JSON)
     |  topic: sensor/mpu6050
     v
Mosquitto MQTT Broker (port 1883)
     |
     |  WiFi network
     v
mqtt_dashboard.py (MacBook)
     |
     v
Live formatted display with timestamps
```

---

## Sample Output

### Publisher (Raspberry Pi)
```
Publishing MPU6050 data to sensor/mpu6050 every 1s
[2026-06-25T19:55:20] Published: accel_z=0.829 gyro_x=-4.267
[2026-06-25T19:55:21] Published: accel_z=0.861 gyro_x=4.587
[2026-06-25T19:55:22] Published: accel_z=0.826 gyro_x=18.122
```

### Dashboard (MacBook)
```
MPU6050 Dashboard — subscribing to sensor/mpu6050
----------------------------------------------------------------------
[19:55:20] ACCEL: x=  0.224 y=  0.247 z=  0.829 | GYRO: x= -4.267 y=  1.213 z=  0.015
[19:55:21] ACCEL: x=  0.246 y=  0.172 z=  0.861 | GYRO: x=  4.587 y=  1.259 z=  1.091
[19:55:22] ACCEL: x=  0.319 y=  0.059 z=  0.826 | GYRO: x= 18.122 y=  8.908 z=  4.000
```

### MQTT JSON Payload
```json
{
    "timestamp": "2026-06-25T19:55:20.959138",
    "accel": {
        "x": 0.224,
        "y": 0.247,
        "z": 0.829
    },
    "gyro": {
        "x": -4.267,
        "y": 1.213,
        "z": 0.015
    }
}
```

---

## MQTT Protocol

MQTT (Message Queuing Telemetry Transport) is the standard protocol for IoT sensor data:

```
Publisher  →  Broker  →  Subscriber
   Pi      →  Pi:1883 →   MacBook

Topic:   sensor/mpu6050
QoS:     0 (fire and forget)
Payload: JSON
Rate:    1Hz (1 message per second)
```

**Why MQTT over HTTP?**
- Lightweight — minimal overhead, ideal for embedded systems
- Pub/Sub — many subscribers can receive same data simultaneously
- Industry standard — used in industrial IoT, smart home, automotive telemetry
- Low bandwidth — JSON payload is under 200 bytes per message

---

## Prerequisites

### On Raspberry Pi
```bash
# Install Mosquitto MQTT broker
sudo apt install mosquitto mosquitto-clients -y
sudo systemctl enable mosquitto

# Configure broker for external connections
sudo nano /etc/mosquitto/mosquitto.conf
# Add:
# listener 1883
# allow_anonymous true

sudo systemctl restart mosquitto

# Install Python MQTT client
pip3 install paho-mqtt --break-system-packages

# Load MPU6050 kernel driver (see 04-mpu6050-driver/)
sudo insmod mpu6050.ko
echo "mpu6050 0x68" | sudo tee /sys/bus/i2c/devices/i2c-1/new_device
```

### On MacBook
```bash
# Create virtual environment
python3 -m venv mqtt_env
source mqtt_env/bin/activate
pip install paho-mqtt

# Install mosquitto client for testing
brew install mosquitto
```

---

## How to Run

### Step 1 — Start publisher on Pi
```bash
python3 mpu6050_publisher.py
```

### Step 2 — Start dashboard on MacBook
```bash
source mqtt_env/bin/activate
python3 mqtt_dashboard.py
```

### Step 3 — Verify with CLI subscriber
```bash
mosquitto_sub -h raspberrypi.local -t "sensor/mpu6050"
```

---

## Files

```
07-iot-mqtt-dashboard/
├── publisher/
│   └── mpu6050_publisher.py   <- reads sysfs, publishes JSON to MQTT
├── subscriber/
│   └── mqtt_dashboard.py      <- subscribes, displays formatted data
└── README.md                  <- this file
```

---

## Extending This Project

```
→ Add cloud broker (HiveMQ, AWS IoT Core) for internet access
→ Add SQLite logging to store historical data
→ Add Flask web dashboard with charts
→ Add threshold alerts — email when gyro exceeds limit
→ Add multiple sensors on same broker
→ Add Grafana dashboard for visualization
```

---

## Skills Demonstrated

- MQTT protocol implementation using paho-mqtt
- Mosquitto broker configuration on embedded Linux
- JSON data serialization for IoT payloads
- Linux kernel driver integration with userspace applications
- Cross-platform data streaming (ARM Linux to macOS)
- Real-time sensor data pipeline design
- IoT system architecture — sensor to dashboard
