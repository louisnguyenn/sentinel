import csv
import time
from pathlib import Path
from pymodbus.client import ModbusTcpClient

REG_CYCLE_COUNT = 10
REG_MACHINE_STATE = 13
REG_INSPECTION_RESULT = 3
REG_MODE_SELECT = 7
REG_ACTIVE_FAULT_CODE = 17

LOG_PATH = Path(__file__).parent.parent / "logs" / "cycles.csv"
LOG_PATH.parent.mkdir(exist_ok=True)

FAULT_STATE = 5

client = ModbusTcpClient(host="localhost", port=5020)
if not client.connect():
    print("Failed to connect to controller")
    exit(1)
print("Logger connected to controller.")

# write header row only if the file doesn't already exist
if not LOG_PATH.exists():
    with open(LOG_PATH, "w", newline="") as f:
        csv.writer(f).writerow(["timestamp", "event_type", "cycle_state", "defective", "fault_code", "mode"])

# write a log into the csv file
def log_event(event_type, cycle_state="", defective="", fault_code="", mode=""):
    with open(LOG_PATH, "a", newline="") as f:
        csv.writer(f).writerow([time.time(), event_type, cycle_state, defective, fault_code, mode])
    print(f"Logged: {event_type}")

previous_cycle_count = None
previous_state = None

while True:
    registers = client.read_holding_registers(0, 17).registers

    current_cycle_count = registers[REG_CYCLE_COUNT]
    current_state = registers[REG_MACHINE_STATE]
    mode = registers[REG_MODE_SELECT]

    if previous_cycle_count is not None and current_cycle_count != previous_cycle_count:
        defective = registers[REG_INSPECTION_RESULT]
        log_event("cycle_complete", cycle_state=current_state, defective=defective, mode=mode)

    if previous_state is not None:
        if current_state == FAULT_STATE and previous_state != FAULT_STATE:
            fault_code = registers[REG_ACTIVE_FAULT_CODE]
            log_event("fault", fault_code=fault_code, mode=mode)

        if current_state != FAULT_STATE and previous_state == FAULT_STATE:
            fault_code = registers[REG_ACTIVE_FAULT_CODE]
            log_event("fault cleared", fault_code=fault_code, mode=mode)

    previous_cycle_count = current_cycle_count
    previous_state = current_state

    time.sleep(0.2)
