# 🏭 Sentinel

> An industrial-grade vision-guided sorting cell, simulated end-to-end in C++17 and Python.

Sentinel simulates a real manufacturing sorting cell: a conveyor moves parts past an inspection
station, a C++ controller runs a PLC-style scan cycle and finite state machine to decide accept or
reject, a lean Python vision module inspects each part, and a Tkinter HMI gives an operator live
control and visibility — all connected over Modbus TCP, the same protocol used by real PLCs, drives,
and sensors on the plant floor. Built to mirror the kind of control software used in real
manufacturing, robotics, and automation systems — not a vision/ML demo with a control loop bolted on.

---

## Demo

### HMI — Live Operator Panel
![HMI Panel](assets/hmi_panel.png)

### Manufacturing Metrics
![Metrics Chart](assets/metrics_trend.png)

### Terminal Output
![Terminal Output](assets/output.png)

---

## Features

- **Simulated field hardware** — photoeye (with debounce), incremental encoder, proximity sensors,
  conveyor motor, and pneumatic diverter cylinder, each modeled with realistic timing and lag
- **PLC-style scan cycle** — fixed-period input scan → logic solve → output scan → housekeeping loop,
  the same architecture real PLC programs run
- **Finite state machine** — explicit cycle states (`IDLE`, `PART_DETECTED`, `AWAIT_RESULT`,
  `DIVERT_ACCEPT`, `DIVERT_REJECT`, `FAULT`) instead of scattered flags
- **Safety &amp; fault logic** — independent watchdog timer, typed fault codes with defined recovery
  paths, e-stop interlock that overrides every other state
- **Operating modes** — `AUTO`, `MANUAL` (operator jog via HMI), and `MAINTENANCE` (interlock bypasses
  are explicit and logged, never silent)
- **Production statistics** — live cycle count, reject count, fault count, and uptime tracked inside
  the controller
- **Modbus TCP integration** — the C++ controller runs a Modbus TCP server; Python vision and the HMI
  are independent Modbus clients, exactly how a vision PC and an HMI panel both talk to the same PLC
  on a real line
- **Lean vision module** — OpenCV-based defect check kept intentionally small (~10% of the project),
  with a heartbeat write the controller's watchdog depends on
- **Virtual HMI** — a Tkinter operator panel: live state, counts, mode select, manual jog, E-STOP,
  current part image and inspection result
- **Manufacturing metrics** — cycle time, reject %, fault %, uptime, throughput, and MTBF computed
  from the run log with pandas

---

## Why I Built This

Vision-guided sorting cells are one of the most common automation problems in manufacturing: manual
inspection is slow and inconsistent, so a camera and a controller take over the accept/reject
decision. Most portfolio projects treat this as a computer-vision problem with some control logic
tacked on. Sentinel inverts that: the control software — the scan cycle, the state machine, the
safety interlocks, the fault recovery, the operating modes — is the actual engineering weight of the
project. The vision piece is deliberately small and well-contained, because on a real line it is too.

This targets the same problem domain as PLC-driven inspection/sorting cells, robotic pick-and-place
stations, and other automated quality-control systems on real production floors.

---

## Tech Stack

| Tool | Purpose |
|---|---|
| C++17 | Controller / control software core |
| CMake 3.20+ | Build system |
| libmodbus | Modbus TCP server (controller side) |
| Google Test | Unit testing for the FSM, watchdog, and fault logic |
| Python 3.10+ | Vision module, HMI, and metrics scripting |
| OpenCV | Defect detection (classical computer vision) |
| pymodbus | Modbus TCP client (vision module and HMI) |
| Tkinter | Virtual HMI operator panel |
| Pillow | Live part-image display in the HMI |
| pandas | Manufacturing metrics (reject %, fault %, uptime, MTBF) |
| matplotlib | Metrics trend charts |
| STL (`thread`, `mutex`, `chrono`, `atomic`) | Scan cycle timing and thread-safe state |

---

## Getting Started

### Prerequisites

- GCC 11+ or Clang 14+
- CMake 3.20+
- libmodbus
- Git
- Python 3.10+ with required libraries

```bash
sudo apt install -y build-essential cmake g++ git libmodbus-dev python3-venv python3-tk
python3 -m venv .venv && source .venv/bin/activate
pip install opencv-python numpy pandas matplotlib pymodbus pytest pillow
```

### Build

```bash
git clone https://github.com/louisnguyenn/sentinel.git
cd sentinel
mkdir -p controller/build && cd controller/build
cmake ..
cmake --build .
```

### Run

```bash
# terminal 1 — start the controller (Modbus TCP server on :502)
./controller/build/controller

# terminal 2 — start the lean vision module
source .venv/bin/activate
python3 vision/inspector.py

# terminal 3 — start the virtual HMI
python3 hmi/panel.py

# or, all at once:
./run.sh
```

### Analyse Metrics

```bash
# after a run, compute cycle time, reject %, fault %, uptime, throughput, MTBF
python3 scripts/analyse_metrics.py
# saves logs/metrics_trend.png
```

### Run Tests

```bash
cd controller/build
ctest --output-on-failure

# python-side tests
pytest vision/tests
```

---

## Project Structure

```
sentinel/
├── CMakeLists.txt
├── run.sh                          ← starts controller + vision + HMI together
├── controller/
│   ├── include/
│   │   ├── photoeye.hpp
│   │   ├── encoder.hpp
│   │   ├── proximity_sensor.hpp
│   │   ├── conveyor_motor.hpp
│   │   ├── diverter_cylinder.hpp
│   │   ├── conveyor_line.hpp       ← owns all simulated field devices
│   │   ├── watchdog.hpp
│   │   └── controller.hpp          ← scan cycle, FSM, safety logic, stats
│   ├── src/
│   │   ├── main.cpp
│   │   ├── conveyor_line.cpp
│   │   └── controller.cpp
│   └── tests/
│       └── test_controller.cpp     ← FSM, watchdog, fault-recovery unit tests
├── vision/
│   ├── inspector.py                ← lean OpenCV defect check + Modbus client
│   └── tests/
│       └── test_inspector.py
├── hmi/
│   └── panel.py                    ← Tkinter operator panel
├── scripts/
│   └── analyse_metrics.py          ← pandas/matplotlib KPI report
├── data/
│   └── sample_parts/               ← OK / defective sample images
├── logs/
│   └── cycles.csv                  ← one row per cycle: state, result, fault code, mode
└── assets/                         ← screenshots for this README
```

---

## Key Concepts Demonstrated

- **Finite state machine design** — `CycleState` and `OperatingMode` as explicit enums driving all
  control logic, instead of scattered booleans
- **PLC-style fixed-period scan cycle** — `inputScan()` → `logicSolve()` → `outputScan()` →
  `housekeeping()`, run on a fixed timer
- **Watchdog / heartbeat pattern** — the controller detects a stalled vision process independently of
  the inspection logic itself
- **Typed fault codes with recovery paths** — `FaultCode` enum, each with a defined reset condition,
  never a silent catch-all fault state
- **Hardware simulation with realistic timing** — debounce, ramp-up/ramp-down, and finite actuation
  time modeled explicitly, not treated as instantaneous
- **Modbus TCP as an integration layer** — a documented holding-register map instead of a REST API or
  message broker, matching real plant-floor communication
- **OOP design** — `ConveyorLine`, `Controller`, `Watchdog`, and each hardware class as decoupled,
  independently testable units
- **std::chrono &amp; std::thread** — real-time scan cycle timing and non-blocking I/O
- **Google Test** — unit tests for FSM transitions, watchdog timeout behaviour, and fault recovery
- **pandas + matplotlib** — deriving reject %, fault %, uptime, throughput, and MTBF from a raw
  per-cycle log

---

## Sample Modbus Register Map

| Reg | Name | Written By | Meaning |
|---|---|---|---|
| 0 | `PHOTOEYE` | Controller | 1 = part present in inspection zone |
| 2 | `TRIGGER_CAPTURE` | Controller | Pulses high when vision should capture/classify |
| 3 | `INSPECTION_RESULT` | Vision | 0 = OK, 1 = defective |
| 4 | `VISION_HEARTBEAT` | Vision | Incremented every vision cycle (watchdog input) |
| 7 | `MODE_SELECT` | HMI | 0 = Auto, 1 = Manual, 2 = Maintenance |
| 8 | `ESTOP` | HMI | 1 = emergency stop active |
| 13 | `MACHINE_STATE` | Controller | Current FSM state, decoded by the HMI for display |

*(Full 16-register map documented in `controller/include/controller.hpp`.)*

---

## Credits
Louis Nguyen
