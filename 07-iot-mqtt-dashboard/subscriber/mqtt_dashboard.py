import paho.mqtt.client as mqtt
import json
from datetime import datetime

BROKER = "raspberrypi.local"
TOPIC = "sensor/mpu6050"

def on_connect(client, userdata, flags, reason_code, properties):
    print(f"Connected to broker")
    client.subscribe(TOPIC)

def on_message(client, userdata, msg):
    data = json.loads(msg.payload.decode())
    ts = data['timestamp'][11:19]
    ax = data['accel']['x']
    ay = data['accel']['y']
    az = data['accel']['z']
    gx = data['gyro']['x']
    gy = data['gyro']['y']
    gz = data['gyro']['z']
    print(f"[{ts}] ACCEL: x={ax:7.3f} y={ay:7.3f} z={az:7.3f} | "
          f"GYRO: x={gx:7.3f} y={gy:7.3f} z={gz:7.3f}")

client = mqtt.Client(mqtt.CallbackAPIVersion.VERSION2)
client.on_connect = on_connect
client.on_message = on_message
client.connect(BROKER, 1883)

print(f"MPU6050 Dashboard — subscribing to {TOPIC}")
print("-" * 70)
client.loop_forever()
