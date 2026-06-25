import paho.mqtt.client as mqtt
import time
import json
from datetime import datetime

BROKER = "localhost"
PORT = 1883
TOPIC = "sensor/mpu6050"
INTERVAL = 1

SYSFS_BASE = "/sys/kernel/mpu6050"

def read_sensor(filename):
    with open(f"{SYSFS_BASE}/{filename}") as f:
        return float(f.read().strip())

def on_connect(client, userdata, flags, reason_code, properties):
    print(f"Connected to MQTT broker: {reason_code}")

client = mqtt.Client(mqtt.CallbackAPIVersion.VERSION2)
client.on_connect = on_connect
client.connect(BROKER, PORT)
client.loop_start()

print(f"Publishing MPU6050 data to {TOPIC} every {INTERVAL}s")
print("Press Ctrl+C to stop\n")

try:
    while True:
        payload = {
            "timestamp": datetime.now().isoformat(),
            "accel": {
                "x": read_sensor("accel_x"),
                "y": read_sensor("accel_y"),
                "z": read_sensor("accel_z")
            },
            "gyro": {
                "x": read_sensor("gyro_x"),
                "y": read_sensor("gyro_y"),
                "z": read_sensor("gyro_z")
            }
        }
        client.publish(TOPIC, json.dumps(payload))
        print(f"[{payload['timestamp']}] Published: "
              f"accel_z={payload['accel']['z']} "
              f"gyro_x={payload['gyro']['x']}")
        time.sleep(INTERVAL)

except KeyboardInterrupt:
    print("\nStopping publisher")
    client.loop_stop()
    client.disconnect()
