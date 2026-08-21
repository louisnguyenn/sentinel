# Phase 5 — Virtual HMI

## HMI = Human-Machine Interface

## Recap
Two Modbus clients existed before this phase:
- **Controller (C++)** — the Modbus server, owns the register array
- **Inspector (Python)** — a machine talking to the controller (vision)

The HMI is the first client meant for an actual human — same protocol, same
register map, different purpose: visibility and control for an operator,
not automated decision-making.

## The 3 things the HMI needs to do

1. **Poll and display**
   - read the register map
   - read on a timer
   - display as labels/colors, not raw numbers

2. **Write commands**
   - mode selection (Manual, Auto, Maintenance), manual jog, e-stop,
     fault reset — full handling already built into `readInputRegisters()`
     from earlier phases
   - writing registers is how the HMI (as a Modbus client) commands the
     controller — no new controller-side plumbing needed, just another
     writer to registers that already have complete, tested behavior

3. **Stay responsive while polling**
   - a Tkinter app needs to run its own continuous loop to stay
     responsive — redrawing, handling clicks, not freezing
   - Tkinter has its own event loop; can't just `while True:` poll inside
     it — needs `root.after(ms, callback)`, scheduling like a recurring
     appointment, letting Tkinter's own loop stay in control
   - the HMI runs its own independent poll timer, separate from both the
     controller's scan cycle and vision's poll loop

## General design principle: edge-detection vs. level-sensitivity

- **Edge-detection** — needed when reacting to a level-type signal would
  cause a repeated, unwanted *side effect* for what should be one logical
  event (e.g. vision only capturing once per `TRIGGER_CAPTURE` pulse,
  not once per poll it happens to still read 1)
- **Level-sensitivity** — correct when "reacting" just means *reflecting
  current truth* (e.g. the HMI redisplaying `MACHINE_STATE` every poll,
  even if unchanged — harmless, no side effect)
- Jogging is a level signal read *correctly*, not a bug: "move while held"
  is naturally continuous, not a one-time event, so re-reading `1` every
  tick while the button is held is exactly the desired behavior

## Multi-client Modbus server (the biggest structural change this phase)

The original `ModbusServer` (Phase 3) only supported **one** client —
`modbus_tcp_accept()` was called once, blocking, at startup. This worked
fine for every test so far (one client at a time) but broke the moment
vision and the HMI both needed to stay connected simultaneously.

**Fix:** rewrote around `select()` — watches the listening socket plus a
list of connected client sockets every `poll()` call, with a zero timeout
(non-blocking). New connections get accepted opportunistically; existing
clients get serviced only if they have a pending request; disconnected
clients get removed from the tracked list.

**Side effect worth noting:** the controller no longer pauses at startup
waiting for a client to connect — it now runs its simulation immediately,
connected or not. Decided this is actually more realistic (a real PLC's
physical process doesn't wait for an HMI to be plugged in), not a
regression.

## Bugs found and fixed this phase

- **Repeated fault-count increments** — holding E-STOP down caused
  `enterFault()` to run on every tick for as long as the button was held,
  each call incrementing `fault_count`. Fixed by making `enterFault()`
  idempotent: guard at the top (`if (m_state == FAULT) return;`) so
  re-entering an already-active fault has no additional effect. Put the
  guard inside `enterFault()` itself so every caller benefits automatically.

- **`REG_RESET_FAULT` never cleared** — HMI only ever wrote `1`, nothing
  wrote it back to `0`. Once clicked, it silently stayed "on" forever,
  causing `requestFaultReset()` to fire on every subsequent tick. Fixed by
  mirroring the internal `m_fault_reset_requested` flag back out in
  `writeOutputRegisters()`, so the register always reflects current
  controller state rather than being a write-only, never-reset flag.

- **Fault recovery structurally unreachable outside Auto mode** — the
  `m_mode != AUTO` early-return in `logicSolve()` happened *before* the
  switch statement containing the `FAULT` case, meaning
  `attemptFaultReset()` could only ever be reached while in Auto mode.
  Manual/Maintenance could enter a fault but never clear one. Fixed by
  moving the `FAULT` check above the mode-gate entirely — fault handling
  should be mode-agnostic, since a fault represents a safety condition
  that transcends whichever mode was active when it happened.

- **Reversed reset-order bug** — clicking RESET FAULT before releasing
  E-STOP still worked, when it should require release-then-reset in that
  order. Root cause: the `m_estop_active` check at the top of
  `logicSolve()` re-triggers `enterFault()` (harmlessly, due to the
  idempotent guard) and `return`s *before* execution ever reaches the
  `FAULT`-handling block — so a reset request made while e-stop is still
  active never actually gets consumed or discarded, it just sits pending
  until e-stop later clears, then fires immediately with no fresh click.
  Fixed by explicitly discarding `m_fault_reset_requested` inside the
  e-stop check itself, so a stale request can't silently carry over —
  reset must be explicitly re-clicked after e-stop is actually released.

  **Lesson worth remembering:** the bug wasn't in the function being
  stared at (`attemptFaultReset()`) — it was in a completely different,
  seemingly unrelated check earlier in `logicSolve()` that was silently
  preventing the "fixed" function from ever running in this specific
  scenario. When a fix doesn't work, worth asking not just "is my fix
  correct" but "is the code I fixed even being reached here."

## Design decisions made this phase

- **E-STOP release**: single click to release the button state
  (`REG_ESTOP = 0`), not a simulated mechanical latch — acceptable because
  a *second*, independent action (RESET FAULT) is still required to
  actually resume operation, giving a real two-step safety property even
  though neither individual step alone simulates a physical latch.
- **No classical-CV fallback if the ML model is missing** (carried over
  from Phase 4) — fail loudly rather than silently degrade.
- **Reset Fault visual feedback**: a brief button flash on click, not a
  "Resetting..." message — chosen because the reset either succeeds or is
  rejected in the same instant; there's no real in-progress period to
  represent, so implying one would be dishonest about what's actually
  happening.
  