import time
import csv
import subprocess
from datetime import datetime

def read_sysfs(path):
    with open(path, 'r') as f:
        return float(f.read().strip())

BASE = "/sys/kernel/mpu6050"
CSV_FILE = "mpu6050_data.csv"

with open(CSV_FILE, 'w', newline='') as f:
    writer = csv.writer(f)
    writer.writerow(['timestamp','accel_x','accel_y','accel_z',
                     'gyro_x','gyro_y','gyro_z'])
    
    print("Logging MPU6050 data... Ctrl+C to stop")
    try:
        while True:
            row = [
                datetime.now().isoformat(),
                read_sysfs(f"{BASE}/accel_x"),
                read_sysfs(f"{BASE}/accel_y"),
                read_sysfs(f"{BASE}/accel_z"),
                read_sysfs(f"{BASE}/gyro_x"),
                read_sysfs(f"{BASE}/gyro_y"),
                read_sysfs(f"{BASE}/gyro_z"),
            ]
            writer.writerow(row)
            f.flush()
            print(f"accel: {row[1]:6.3f} {row[2]:6.3f} {row[3]:6.3f} | "
                  f"gyro: {row[4]:6.3f} {row[5]:6.3f} {row[6]:6.3f}")
            time.sleep(0.1)
    except KeyboardInterrupt:
        print(f"\nSaved {CSV_FILE}")
