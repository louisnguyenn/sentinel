from pymodbus.client import ModbusTcpClient
from defect_check import detect_surface_defect, capture_current_part_image, classify_with_model
import time
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).parent.parent))
from shared_registers import *
print("REG_COUNT is:", REG_COUNT)

client = ModbusTcpClient(host="localhost", port=5020) # match controller port
connected = client.connect()
if not connected:
    print("Failed to connect to Modbus server")
    exit(1)
print("Connected to controller.")

prev_trigger = 0
heartbeat = 0
result_seq = 0
loop_count = 0

while True:
    result = client.read_holding_registers(address=0, count=REG_COUNT)
    # print("Raw result:", result)
    # print("Registers:", result.registers)
    # print("Length:", len(result.registers))

    registers = result.registers
    curr_trigger = registers[REG_TRIGGER_CAPTURE]

    if curr_trigger == 1 and prev_trigger == 0:
        print("Trigger detected - capturing and classifying...")
        image_path = capture_current_part_image()
        defective = classify_with_model(image_path)
        print(f"    {image_path} -> defective={defective}")

        client.write_register(REG_INSPECTION_RESULT, int(defective))
        result_seq += 1 # track new result
        client.write_register(REG_RESULT_SEQ, result_seq)

    prev_trigger = curr_trigger

    heartbeat = (heartbeat + 1) % 65535 # writes unconditionally
    client.write_register(REG_VISION_HEARTBEAT, heartbeat)

    loop_count += 1
    if loop_count % 100 == 00:
        print(f"    ...still polling (loop {loop_count}, heartbeat={heartbeat})")

    time.sleep(0.02)
