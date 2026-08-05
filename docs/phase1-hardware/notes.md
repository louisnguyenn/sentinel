# Phase 1 — Simulated Field Hardware

## Parts of a conveyor line
photoeye → encoder → proximity sensor → conveyor motor → diverter cylinder

## Photoeye
- Electric eye (beam of light/laser)
- Shines from one side to a sensor on the other side
- Part blocks the beam → something is there

**Debounce**
- Prevents noise on the sensor → prevents reporting from the flickering of the beam
- Waits until a state has held steady for a little while before reporting it

## Encoder
- Small wheel or disk attached to a motor shaft with stripes on it
- As the motor spins, the encoder counts how many stripes go by
- Each stripe = one pulse
- Speed is calculated from # of pulses over # of time

**Fractional pulse accumulator**
- Accumulates fractional pulses so slow speeds are still counted accurately over time
  (instead of throwing away partial pulses every tick)

## Proximity sensor
- Small sensor at each end of the diverter arm's travel path
- One sensor detects the arm is fully retracted, the other detects full extension
- Why? If the sensor doesn't detect full retraction/extension, the proximity sensor
  can flag a fault (e.g. jammed cylinder)

## Conveyor motor
- Acceleration / ramp-up, doesn't jump straight to speed
- If it jumped to speed instantly, encoder pulses would also jump → unrealistic behavior
- `v = a · dt` → speed formula (used to ramp speed up/down gradually per tick)

## Diverter cylinder
- Air-powered (pneumatic) piston
- Activates (extends) when a part is determined to be defective

## Putting it together: ConveyorLine
- Holds all of the parts together
- Moves everything forward each tick (`update(dt_s)`)
