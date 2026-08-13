from pymodbus.client import ModbusTcpClient
import time

REG_TRIGGER_CAPTURE = 2
REG_INSPECTION_RESULT = 3
REG_VISION_HEARTBEAT = 4
REG_RESULT_SEQ = 16

client = ModbusTcpClient("localhost", 5020) # match controller port
client.connect()

prev_trigger = 0
heartbeat = 0
result_seq = 0

while True:
    registers = client.read_holding_registers(0, 17).registers
    curr_trigger = registers[REG_TRIGGER_CAPTURE]

    if curr_trigger == 1 and prev_trigger == 0:
        # TODO: capture/load image
        # TODO: defective = ...

        client.write_register(REG_INSPECTION_RESULT, int(defective))
        result_seq += 1 # track new result
        client.write_registers(REG_RESULT_SEQ, result_seq)

    prev_trigger = curr_trigger

    heartbeat = (heartbeat + 1) % 65535 # writes unconditionally
    client.write_registers(REG_VISION_HEARTBEAT, heartbeat)

    time.sleep(0.02)
