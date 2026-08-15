from pymodbus.client import ModbusTcpClient
from defect_check import detect_surface_defect, capture_current_part_image, classify_with_model
import time

REG_TRIGGER_CAPTURE = 2
REG_INSPECTION_RESULT = 3
REG_VISION_HEARTBEAT = 4
REG_RESULT_SEQ = 16

client = ModbusTcpClient(host="localhost", port=5020) # match controller port
client.connect()

prev_trigger = 0
heartbeat = 0
result_seq = 0

while True:
    registers = client.read_holding_registers(address=0, count=17).registers
    curr_trigger = registers[REG_TRIGGER_CAPTURE]

    if curr_trigger == 1 and prev_trigger == 0:
        image_path = capture_current_part_image()
        defective = classify_with_model(image_path)

        client.write_register(REG_INSPECTION_RESULT, int(defective))
        result_seq += 1 # track new result
        client.write_register(REG_RESULT_SEQ, result_seq)

    prev_trigger = curr_trigger

    heartbeat = (heartbeat + 1) % 65535 # writes unconditionally
    client.write_register(REG_VISION_HEARTBEAT, heartbeat)

    time.sleep(0.02)
