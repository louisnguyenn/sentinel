import tkinter as tk
from pymodbus.client import ModbusTcpClient

REG_MACHINE_STATE = 13
REG_CYCLE_COUNT = 10
REG_REJECT_COUNT = 11
REG_FAULT_COUNT = 12
REG_MODE_SELECT = 7
REG_ESTOP = 8

client = ModbusTcpClient(host="localhost", port=5020)
connected = client.connect()
if not connected:
    print("Failed to connect to controller")
    exit(1)
print("HMI connected to controller.")

root = tk.Tk()
root.title("Sentinel - Operator Panel")
root.geometry("400x300")

state_label = tk.Label(root, text="STATE: -", font=("Courier, 16"))
state_label.pack(pady=10)

counts_label = tk.Label(root, text="Cycle: 0   Reject: 0   Fault: 0", font=("Courier", 12))
counts_label.pack(pady=5)

def poll_and_update():
    registers = client.read_holding_registers(address=0, count=17).registers

    # TODO: decode registers[REG_MACHINE_STATE] into a human-readable
    # string and set it on state_label using state_label.config(text=...)

    # TODO: read cycle/reject/fault counts and update counts_label the
    # same way

    root.after(200, poll_and_update) # schedule this same function again in 200ms

poll_and_update()
root.mainloop()
