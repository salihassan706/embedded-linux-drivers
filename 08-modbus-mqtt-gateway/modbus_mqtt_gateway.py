import asyncio
import json
import paho.mqtt.client as mqtt
from datetime import datetime
from pymodbus.client import AsyncModbusTcpClient

MQTT_BROKER = "localhost"
MQTT_TOPIC  = "modbus/sensors"

mqtt_client = mqtt.Client(mqtt.CallbackAPIVersion.VERSION2)
mqtt_client.connect(MQTT_BROKER, 1883)
mqtt_client.loop_start()

async def gateway():
    client = AsyncModbusTcpClient("localhost", port=5020)
    await client.connect()
    print("Modbus-MQTT Gateway started")
    print(f"Polling Modbus slave → publishing to {MQTT_TOPIC}")
    print("-" * 50)

    while True:
        result = await client.read_holding_registers(1, count=4, slave=1)
        if not result.isError():
            regs = result.registers
            payload = {
                "timestamp": datetime.now().isoformat(),
                "temperature": regs[0] / 100,
                "humidity":    regs[1] / 100,
                "pressure":    regs[2] / 10,
                "status":      "OK" if regs[3] == 1 else "FAULT"
            }
            mqtt_client.publish(MQTT_TOPIC, json.dumps(payload))
            print(f"[{payload['timestamp'][11:19]}] "
                  f"temp={payload['temperature']:.2f}C "
                  f"humidity={payload['humidity']:.2f}% "
                  f"pressure={payload['pressure']:.1f}hPa "
                  f"status={payload['status']}")
        await asyncio.sleep(1)

if __name__ == "__main__":
    asyncio.run(gateway())
