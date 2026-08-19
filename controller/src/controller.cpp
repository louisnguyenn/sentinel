#include "controller.hpp"

// public methods
sentinel::Controller::Controller(ConveyorLine& line) : m_line{line}
{
}

/// @brief entry point - runs full scan cycle in order: input (inputScan) → decide (logicSolve) →
/// output (outputScan) → housekeeping
/// @param dt_s
void sentinel::Controller::tick(double dt_s)
{
    inputScan();
    logicSolve();
    outputScan();
    housekeeping();
}

void sentinel::Controller::setEstop(bool active)
{
    m_estop_active = active;
}

void sentinel::Controller::requestFaultReset()
{
    m_fault_reset_requested = true;
}

void sentinel::Controller::setMode(OperatingMode mode)
{
    m_mode = mode;
}

/// @brief Updates members on part inspection result
/// @param defective - 0 = OK, 1 = defective
void sentinel::Controller::submitInspectionResult(bool defective)
{
    m_has_inspection_result = true;
    m_inspection_result_defective = defective;
}

void sentinel::Controller::feedVisionHeartbeat(uint16_t heartbeat_value)
{
    m_watchdog.feed(heartbeat_value);
}

sentinel::CycleState sentinel::Controller::state() const
{
    return m_state;
}

sentinel::OperatingMode sentinel::Controller::mode() const
{
    return m_mode;
}

sentinel::FaultCode sentinel::Controller::activeFault() const
{
    return m_active_fault;
}

const sentinel::Stats& sentinel::Controller::stats() const
{
    return m_stats;
}

/// @brief Reads input registers from Modbus and calls corresponding methods
/// @param registers
void sentinel::Controller::readInputRegisters(const uint16_t registers[REG_COUNT])
{
    // set estop
    setEstop(registers[REG_ESTOP] != 0); // return boolean

    // mode selection
    switch (registers[REG_MODE_SELECT])
    {
        case 0:
            setMode(OperatingMode::AUTO);
            break;

        case 1:
            setMode(OperatingMode::MANUAL);
            break;

        case 2:
            setMode(OperatingMode::MAINTENANCE);
            break;
        default:
            m_invalid_mode_detected = true;
            break;
    }

    // reset fault
    if (registers[REG_RESET_FAULT] != 0)
    {
        requestFaultReset();
    }

    // jogging conveyor
    if (m_mode == OperatingMode::MANUAL && m_active_fault == FaultCode::NONE)
    {
        m_line.jogConveyor(registers[REG_MANUAL_CONVEYOR_JOG] != 0);
    }
    else
    {
        m_line.jogConveyor(false); // never jogging outside of Manual mode and active fault
    }

    // inspection result
    if (m_state == CycleState::AWAIT_RESULT)
    {
        // ensure new baseline to avoid leftover data
        if (!m_result_seq_baseline_captured)
        {
            // First time seeing AWAIT_RESULT since it started - track whatever sequence number is
            // currenting stored as the baseline so leftover data from previous part cannot be
            // mistaken as new data
            m_last_result_seq = registers[REG_RESULT_SEQ];
            m_result_seq_baseline_captured = true;
        }

        // check for new result
        if (registers[REG_RESULT_SEQ] != m_last_result_seq)
        {
            m_last_result_seq = registers[REG_RESULT_SEQ];
            submitInspectionResult(registers[REG_INSPECTION_RESULT] != 0);
        }
    }

    // vision heartbeat for watchdog
    feedVisionHeartbeat(registers[REG_VISION_HEARTBEAT]);
}

void sentinel::Controller::writeOutputRegisters(uint16_t registers[REG_COUNT]) const
{
    registers[REG_MACHINE_STATE] = static_cast<uint16_t>(state());

    if (state() == CycleState::PART_DETECTED ||
        (state() == CycleState::AWAIT_RESULT && !m_has_inspection_result))
    {
        registers[REG_TRIGGER_CAPTURE] = 1;
    }
    else
    {
        registers[REG_TRIGGER_CAPTURE] = 0;
    }

    registers[REG_CYCLE_COUNT] = m_stats.cycle_count;
    registers[REG_REJECT_COUNT] = m_stats.reject_count;
    registers[REG_FAULT_COUNT] = m_stats.fault_count;
    registers[REG_PHOTOEYE] = m_photoeye_snapshot;
    registers[REG_DIVERTER_CMD] = m_last_diverter_cmd;
    registers[REG_DIVERTER_FEEDBACK] = static_cast<uint16_t>(m_line.diverterPosition());
    registers[REG_RESET_FAULT] = m_fault_reset_requested;
}

// private methods

/// @brief taking a 'snapshot' - get photoeye's state
void sentinel::Controller::inputScan()
{
    m_photoeye_previous = m_photoeye_snapshot; // save the snapshot first before updating
    m_photoeye_snapshot = m_line.photoeyeBlocked();
}

void sentinel::Controller::logicSolve()
{
    // check mode
    if (m_invalid_mode_detected == true)
    {
        enterFault(FaultCode::INVALID_MODE_REQUEST);
        m_invalid_mode_detected = false;
        return;
    }

    // check e-stop
    if (m_estop_active == true)
    {
        enterFault(FaultCode::ESTOP);
        m_fault_reset_requested = false;
        return;
    }

    if (m_state == CycleState::FAULT)
    {
        if (m_fault_reset_requested)
        {
            attemptFaultReset();
        }

        return;
    }

    // check if on auto
    if (m_mode != OperatingMode::AUTO)
    {
        return;
    }

    switch (m_state)
    {
        case CycleState::IDLE:
            // check photoeye if there is a part
            if (m_photoeye_snapshot == true && m_photoeye_previous == false)
            {
                m_state = CycleState::PART_DETECTED;
            }
            break;
        case CycleState::PART_DETECTED:
            m_has_inspection_result = false; // no result yet
            m_result_seq_baseline_captured =
                false; // next readInputRegisters() call will establish a new baseline
            m_watchdog.reset();
            m_state = CycleState::AWAIT_RESULT;
            break;
        case CycleState::AWAIT_RESULT:
            // check if watchdog is still recieving updates
            if (m_watchdog.timedOut() == true)
            {
                enterFault(FaultCode::VISION_TIMEOUT);
                return;
            }

            if (m_has_inspection_result == true)
            {
                // check if result is defective
                if (m_inspection_result_defective == true)
                {
                    m_state = CycleState::DIVERT_REJECT;
                }
                else
                {
                    m_state = CycleState::DIVERT_ACCEPT;
                }
            }
            break;
        case CycleState::DIVERT_REJECT:
            m_line.commandDiverter(true); // extend diverter
            m_last_diverter_cmd = true;

            if (m_line.diverterExtended() == true)
            {
                m_stats.reject_count++; // increment reject count
                m_state = CycleState::IDLE;
            }
            break;
        case CycleState::DIVERT_ACCEPT:
            m_line.commandDiverter(false); // retract diverter
            m_last_diverter_cmd = false;

            if (m_line.diverterRetracted() == true)
            {
                m_state = CycleState::IDLE;
            }
            break;
    }
}

// TODO: finish modbus in phase 3 first
void sentinel::Controller::outputScan()
{
}

// TODO: increment cycle_count once we track previous state
void sentinel::Controller::housekeeping()
{
}

void sentinel::Controller::enterFault(FaultCode code)
{
    // check if already in fault
    if (m_state == CycleState::FAULT)
    {
        return;
    }

    m_state = CycleState::FAULT;
    m_active_fault = code;
    m_stats.fault_count++;
    m_line.commandDiverter(false); // retract diverter
    m_last_diverter_cmd = false;
    m_line.setConveyorRunning(false); // turn off motor
}

void sentinel::Controller::attemptFaultReset()
{
    m_fault_reset_requested = false;

    if (m_active_fault == FaultCode::ESTOP && m_estop_active)
    {
        return;
    }
    else if (m_active_fault == FaultCode::VISION_TIMEOUT)
    {
        m_watchdog.reset(); // reset watchdog to clear fault
    }

    // reset fault
    m_active_fault = FaultCode::NONE;
    m_state = CycleState::IDLE;

    m_line.setConveyorRunning(true); // set conveyor running
}
