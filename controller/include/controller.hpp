#ifndef SENTINEL_CONTROLLER_HPP
#define SENTINEL_CONTROLLER_HPP

#include "conveyor_line.hpp"
#include "modbus_registers.hpp"
#include "watchdog.hpp"
#include <cstdint>

namespace sentinel
{

/// Where the automatic cycle currently is.
enum class CycleState
{
    IDLE,
    PART_DETECTED,
    AWAIT_RESULT,
    DIVERT_ACCEPT,
    DIVERT_REJECT,
    FAULT
};

/// How much automation is currently allowed to run.
enum class OperatingMode
{
    AUTO,
    MANUAL,
    MAINTENANCE
};

/// What specifically went wrong, when in FAULT.
enum class FaultCode
{
    NONE,
    ESTOP,
    VISION_TIMEOUT,
    DIVERTER_JAM
};

/// Running production counters — the numbers a plant floor cares about.
struct Stats
{
    uint64_t cycle_count = 0;
    uint64_t reject_count = 0;
    uint64_t fault_count = 0;
};

/// The "brain": reads the simulated plant, runs the FSM and safety
/// logic, and drives the simulated plant's actuators. One tick() call
/// represents one full scan cycle.
class Controller
{
  public:
    explicit Controller(ConveyorLine& line);

    /// Advance the controller by one scan cycle. dt_s should match
    /// whatever tick rate the ConveyorLine itself is being updated at.
    void tick(double dt_s);

    // --- Inputs from the "outside world" ---
    // (In Phase 3, these get driven by Modbus register reads instead
    // of being called directly. For now, your test harness calls them.)
    void setEstop(bool active);
    void requestFaultReset();
    void setMode(OperatingMode mode);
    void submitInspectionResult(bool defective);
    void feedVisionHeartbeat(uint16_t heartbeat_value);

    // --- Observability, for the HMI and logging later ---
    CycleState state() const;
    OperatingMode mode() const;
    FaultCode activeFault() const;
    const Stats& stats() const;

    /// Translation layer for server-client communication using Modbus
    /// Reads registers written by external clients (HMI, vision) and
    /// drives the controller's existing setters accordingly. Call this
    /// once per scan cycle, before tick().
    void readInputRegisters(const uint16_t registers[REG_COUNT]);

    /// Writes the controller's current internal state out into the
    /// registers array, for external clients to read. Call this once
    /// per scan cycle, after tick().
    void writeOutputRegisters(uint16_t registers[REG_COUNT]) const;

  private:
    void inputScan();
    void logicSolve();
    void outputScan();
    void housekeeping();

    void enterFault(FaultCode code);
    void attemptFaultReset();

    ConveyorLine& m_line;
    Watchdog m_watchdog;
    Stats m_stats;

    CycleState m_state = CycleState::IDLE;
    OperatingMode m_mode = OperatingMode::AUTO;
    FaultCode m_active_fault = FaultCode::NONE;

    bool m_estop_active = false;
    bool m_fault_reset_requested = false;

    bool m_has_inspection_result = false;
    bool m_inspection_result_defective = false;

    // Snapshot of sensor state, captured during inputScan() and used
    // only during logicSolve() — never re-read mid-logic.
    bool m_photoeye_snapshot = false;
    bool m_photoeye_previous = false;
};

} // namespace sentinel

#endif
