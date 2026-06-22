import serial
from datetime import datetime

port = serial.Serial(
    port='/dev/cu.usbserial-0001',
    baudrate=115200,
    timeout=1
)

print("Waiting for sensor data...")
while True:
    data = port.read(256)
    if data:
        text = data.decode('utf-8', errors='ignore').strip()
        if text:
            timestamp = datetime.now().strftime("%H:%M:%S")
            print(f"[{timestamp}] {text}")
