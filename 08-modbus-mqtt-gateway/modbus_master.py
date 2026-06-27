import asyncio
from pymodbus.client import AsyncModbusTcpClient

async def read_registers():
    client = AsyncModbusTcpClient("localhost", port=5020)
    await client.connect()
    print("Connected to Modbus slave")
    print("-" * 50)

    while True:
        result = await client.read_holding_registers(1, count=4, slave=1)
        if not result.isError():
            regs = result.registers
            temp     = regs[0] / 100
            humidity = regs[1] / 100
            pressure = regs[2] / 10
            status   = "OK" if regs[3] == 1 else "FAULT"
            print(f"Temperature: {temp:.2f} C  "
                  f"Humidity: {humidity:.2f}%  "
                  f"Pressure: {pressure:.1f} hPa  "
                  f"Status: {status}")
        else:
            print(f"Error: {result}")
        await asyncio.sleep(1)

if __name__ == "__main__":
    asyncio.run(read_registers())
