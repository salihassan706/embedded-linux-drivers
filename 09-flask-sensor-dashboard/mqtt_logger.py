import paho.mqtt.client as mqtt
import sqlite3
import json
from datetime import datetime

DB_PATH = "/home/pi/sensor_data.db"
BROKER  = "localhost"
TOPIC   = "sensor/mpu6050"

def init_db():
    conn = sqlite3.connect(DB_PATH)
    conn.execute('''
        CREATE TABLE IF NOT EXISTS sensor_readings (
            id        INTEGER PRIMARY KEY AUTOINCREMENT,
            timestamp TEXT,
            accel_x   REAL,
            accel_y   REAL,
            accel_z   REAL,
            gyro_x    REAL,
            gyro_y    REAL,
            gyro_z    REAL
        )
    ''')
    conn.commit()
    conn.close()
    print("Database initialized")

def save_reading(data):
    conn = sqlite3.connect(DB_PATH)
    conn.execute('''
        INSERT INTO sensor_readings
        (timestamp, accel_x, accel_y, accel_z, gyro_x, gyro_y, gyro_z)
        VALUES (?, ?, ?, ?, ?, ?, ?)
    ''', (
        data['timestamp'],
        data['accel']['x'],
        data['accel']['y'],
        data['accel']['z'],
        data['gyro']['x'],
        data['gyro']['y'],
        data['gyro']['z'],
    ))
    conn.commit()
    conn.close()

def on_connect(client, userdata, flags, reason_code, properties):
    print(f"Connected to broker")
    client.subscribe(TOPIC)
    print(f"Subscribed to {TOPIC}")

def on_message(client, userdata, msg):
    data = json.loads(msg.payload.decode())
    save_reading(data)
    print(f"[{data['timestamp'][11:19]}] Saved: "
          f"accel_z={data['accel']['z']} "
          f"gyro_x={data['gyro']['x']}")

init_db()
client = mqtt.Client(mqtt.CallbackAPIVersion.VERSION2)
client.on_connect = on_connect
client.on_message = on_message
client.connect(BROKER, 1883)

print("MQTT Logger started — saving to SQLite")
client.loop_forever()
