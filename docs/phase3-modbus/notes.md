# Phase 3 — Modbus TCP Integration

## Modbus
- Acts like a middleman between a C++ process and a Python process
- **Holding registers**: 16-bit numbers, each with an agreed-upon meaning
  (e.g. register 8 always means e-stop)

## Server vs. client
- The **C++ controller is the server** — owns the register array and is always listening
- **Python vision** and **Python HMI** are both **clients**
- Clients talk to each other only indirectly, through the server's registers —
  never directly to each other

## What does this phase actually solve?
- The controller only understands C++ method calls
- Modbus only understands a plain array of numbers (registers)
- Something has to sit between the controller and Modbus to translate specific
  register values into the right method calls — that's what we're building
  (`readInputRegisters()` / `writeOutputRegisters()`)
  