# Sentinel — Project Notes

This folder contains my working notes for each phase of the project: the concepts I had to
learn, the design decisions I made (and why), and the specific bugs I hit along the way. The
goal is partly a personal reference, and partly a record of *how* I actually built this, not
just a description of the finished thing.

Each phase folder contains:
- **`notes.md`** — concepts learned, design decisions, and how each piece works
- **`bugs-and-fixes.md`** — specific bugs I hit, what caused them, and how I found/fixed them

---

## Phases

### [Phase 1 — Simulated Field Hardware](phase1-hardware/notes.md)
Photoeye, encoder, proximity sensor, conveyor motor, and diverter cylinder — modeling real
field devices with realistic timing (debounce, ramp-up, finite actuation time) instead of
treating sensors/actuators as instantaneous booleans.

### [Phase 2 — Controller Core: Scan Cycle, FSM &amp; Safety Logic](phase2-controller/notes.md)
The PLC-style scan cycle, the finite state machine driving the sort cycle, the watchdog
timer, fault codes and recovery, and operating modes (Auto/Manual/Maintenance).

### [Phase 3 — Modbus TCP Integration](phase3-modbus/notes.md)
Connecting the C++ controller (as a Modbus TCP server) to Python vision and the HMI (as
Modbus clients), via a documented holding-register map.

### [Phase 4 — Lean Python Vision Module](phase4-vision/notes.md)
Camera calibration and classical defect detection, kept deliberately small — the vision
module is a supporting piece, not the center of the project.

### [Phase 5 — Virtual HMI](phase5-hmi/notes.md)
A Tkinter operator panel polling the controller's Modbus registers: live state, counts,
mode selection, manual jog, and e-stop.

### [Phase 6 — Manufacturing Metrics &amp; Logging](phase6-metrics/notes.md)
Turning a raw per-cycle log into real manufacturing KPIs: cycle time, reject %, fault %,
uptime, throughput, and MTBF.

### [Phase 7 — Testing &amp; Packaging](phase7-testing/notes.md)
GoogleTest coverage for the FSM and safety logic, pytest for the vision module, and how
everything runs together.

---

## Other useful references

- Root [`README.md`](../README.md) — project overview, build/run instructions
- [`modbus_registers.hpp`](../controller/include/modbus_registers.hpp) — the authoritative
  register map (docs here may lag behind; the header is the source of truth)
- CI workflow: [`.github/workflows/ci.yml`](../.github/workflows/ci.yml)