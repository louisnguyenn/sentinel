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

