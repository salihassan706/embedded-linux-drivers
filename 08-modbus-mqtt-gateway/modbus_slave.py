import asyncio
import random
from pymodbus.server import StartAsyncTcpServer
from pymodbus.datastore import ModbusSlaveContext, ModbusServerContext
from pymodbus.datastore import ModbusSequentialDataBlock

def create_context():
    block = ModbusSequentialDataBlock(1, [2651, 6500, 10132, 1])
    slave = ModbusSlaveContext(hr=block)
    return ModbusServerContext(slaves=slave, single=True)

async def update_registers(context):
    while True:
        await asyncio.sleep(2)
        temp     = 2600 + random.randint(0, 100)
        humidity = 6000 + random.randint(0, 1000)
        pressure = 10100 + random.randint(0, 50)
        context[0x00].setValues(3, 1, [temp, humidity, pressure, 1])
        print(f"[Slave] temp={temp/100:.2f}C "
              f"humidity={humidity/100:.2f}% "
              f"pressure={pressure/10:.1f}hPa")

async def main():
    context = create_context()
    print("Modbus TCP slave running on localhost:5020")
    await asyncio.gather(
        StartAsyncTcpServer(context=context, address=("localhost", 5020)),
        update_registers(context)
    )

if __name__ == "__main__":
    asyncio.run(main())
