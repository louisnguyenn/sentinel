import tkinter as tk
from pymodbus.client import ModbusTcpClient

REG_MACHINE_STATE = 13
REG_CYCLE_COUNT = 10
REG_REJECT_COUNT = 11
REG_FAULT_COUNT = 12
REG_MODE_SELECT = 7
REG_ESTOP = 8
REG_RESET_FAULT = 9
REG_MANUAL_CONVEYOR_JOG = 14

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
state_label.pack(pady=10)   # create widget with vertical padding of 10

counts_label = tk.Label(root, text="Cycle: 0   Reject: 0   Fault: 0", font=("Courier", 12))
counts_label.pack(pady=5)   # create width with padding y of 5

status_banner = tk.Label(root, text="AUTO", bg="green", fg="white",
                           font=("Courier", 18, "bold"), width=20)
status_banner.pack(pady=10)

def on_estop_click():
    client.write_register(REG_ESTOP, 1)

estop_button = tk.Button(root, text="E-STOP", bg="red", fg="white",
                           font=("Courier", 16, "bold"), command=on_estop_click)
estop_button.pack(pady=10)

def on_jog_press(event):
    client.write_register(REG_MANUAL_CONVEYOR_JOG, 1)

def on_jog_release(event):
    client.write_register(REG_MANUAL_CONVEYOR_JOG, 0)

jog_button = tk.Button(root, text="JOG CONVEYOR", font=("Courier", 14))
jog_button.pack(pady=10)
jog_button.bind("<ButtonPress-1>", on_jog_press)
jog_button.bind("<ButtonRelease-1>", on_jog_release)

def get_status_banner(state: int, mode: int) -> tuple[str, str]:
    """Returns (label_text, background_color)."""
    if state == 5:
        return ("FAULT", "red")
    elif mode == 0:
        return ("AUTO", "green")
    elif mode == 1:
        return ("MANUAL", "orange")
    elif mode == 2:
        return ("MAINTENANCE", "yellow")

def on_estop_release():
    client.write_register(REG_ESTOP, 0)

estop_release_button = tk.Button(root, text="RELEASE E-STOP", bg="gray", fg="white", font=("Courier", 12), command=on_estop_release)
estop_release_button.pack(pady=5)

def on_reset_fault():
        client.write_register(REG_RESET_FAULT, 1)

reset_button = tk.Button(root, text="RESET FAULT", bg="gray", fg="white", font=("Courier", 12), command=on_reset_fault)
reset_button.pack(pady=5)

def set_mode(mode_value: int):
    client.write_register(REG_MODE_SELECT, mode_value)

# container widget - buttons side by side
mode_frame = tk.Frame(root)
mode_frame.pack(pady=10)

auto_button = tk.Button(mode_frame, text="AUTO", width=10,
                          command=lambda: set_mode(0))
auto_button.pack(side="left", padx=5)

manual_button = tk.Button(mode_frame, text="MANUAL", width=10,
                            command=lambda: set_mode(1))
manual_button.pack(side="left", padx=5)

maintenance_button = tk.Button(mode_frame, text="MAINTENANCE", width=10,
                                 command=lambda: set_mode(2))
maintenance_button.pack(side="left", padx=5)

def poll_and_update():
    registers = client.read_holding_registers(address=0, count=17).registers

    STATE_NAMES = {
        0: "IDLE",
        1: "PART_DETECTED",
        2: "AWAIT_RESULT",
        3: "DIVERT_ACCEPT",
        4: "DIVERT_REJECT",
        5: "FAULT"
    }

    # update state with state names
    state_label.config(text=STATE_NAMES.get(registers[REG_MACHINE_STATE], "UNKNOWN"))

    counts_label.config(text=f"Cycle: {registers[REG_CYCLE_COUNT]}   Reject: {registers[REG_REJECT_COUNT]}   Fault: {registers[REG_FAULT_COUNT]}")

    text, color = get_status_banner(registers[REG_MACHINE_STATE], registers[REG_MODE_SELECT])
    status_banner.config(text=text, bg=color)

    root.after(200, poll_and_update) # schedule this same function again in 200ms

poll_and_update()
root.mainloop()
