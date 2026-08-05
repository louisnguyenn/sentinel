# Phase 2 — Controller Core: Scan Cycle, FSM & Safety Logic

## Industrial controller
- Runs on a fixed heartbeat → fixed rhythm: read everything, think, act, repeat
- This is called a **scan cycle**

## The scan cycle
Loops through these steps at a fixed rate (e.g. 10ms per tick):
1. **Input scan** — read every sensor right now (take a snapshot)
2. **Logic solve** — using the snapshot, decide what to do
3. **Output scan** — send out all the commands decided
4. **Housekeeping** — update counters, logs, timers

Every tick calls these in order: input scan → logic solve → output scan → housekeeping

## FSM (finite state machine)
Machine has defined states — can't jump between arbitrary states, only transition
along defined paths:
1. **Idle** — waiting
2. **Part detected** — photoeye saw something, send a request to inspect it
3. **Await result** — waiting to hear back, good or bad
4. **Divert accept / divert reject** — command the diverter accordingly based on the result
5. **Fault** — something went wrong, stop normal flow until the fault is cleared/reset

## Operating modes
Modes the FSM can run under:
1. **Auto** — FSM runs automatically
2. **Manual** — a human directly operates the FSM; automatic transitions are off,
   but safety features (watchdog, e-stop) are still active
3. **Maintenance** — allows bypassing certain interlocks for testing; every bypass
   gets explicitly logged, never silent

## Watchdog
- Like a dead man's switch
- Requires constant updates (heartbeat)
- No updates for too long → something happened (assume the worst, fault out)

## Fault codes and recovery
- Tells us specifically what broke, not just "something broke"
- Every fault code has a proper reset condition
- Entering a fault drives the physical components to a known-safe state
  (e.g. stop conveyor, retract diverter) instead of freezing everything in place

## Level-sensitive vs. edge-sensitive
- **Level-sensitive** — reacts to the *current* state ("is it blocked right now?")
- **Edge-sensitive** — reacts to a *change* in state ("did it just go from false to true?")
- Matters because a level check can re-trigger repeatedly while a condition stays true;
  an edge check only fires once, at the moment of transition
  