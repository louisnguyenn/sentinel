#include "controller.hpp"

// public methods
sentinel::Controller::Controller(ConveyorLine& line) : m_line{line}
{
}

/// @brief entry point - runs full scan cycle in order: input → decide → output → bookkeeping
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
    // TODO: possible error logging / throw error if wrong mode is selected
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
    }

    // reset fault
    if (registers[REG_RESET_FAULT] != 0)
    {
        requestFaultReset();
    }

    // inspection result
    if (m_state == CycleState::AWAIT_RESULT)
    {
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

    // TODO: populate trigger capture, when should the controller want visiont to capture
    if (state() == CycleState::AWAIT_RESULT)
    {
    }

    registers[REG_CYCLE_COUNT] = m_stats.cycle_count;
    registers[REG_REJECT_COUNT] = m_stats.reject_count;
    registers[REG_FAULT_COUNT] = m_stats.fault_count;

    registers[REG_PHOTOEYE] = m_photoeye_snapshot;

    // TODO: write REG_DIVERTER_CMD and REG_DIVERTER_FEEDBACK
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
    // check e-stop
    if (m_estop_active == true)
    {
        enterFault(FaultCode::ESTOP);
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
            m_last_result_seq = registers[REG_RESULT_SEQ];
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

            if (m_line.diverterExtended() == true)
            {
                m_stats.reject_count++; // increment reject count
                m_state = CycleState::IDLE;
            }
            break;
        case CycleState::DIVERT_ACCEPT:
            m_line.commandDiverter(false); // retract diverter

            if (m_line.diverterRetracted() == true)
            {
                m_state = CycleState::IDLE;
            }
            break;
        case CycleState::FAULT:
            if (m_fault_reset_requested == true)
            {
                attemptFaultReset(); // call fault reset
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
    m_state = CycleState::FAULT;
    m_active_fault = code;
    m_stats.fault_count++;
    m_line.commandDiverter(false);    // retract diverter
    m_line.setConveyorRunning(false); // turn off motor
}

void sentinel::Controller::attemptFaultReset()
{
    if (m_active_fault == FaultCode::ESTOP)
    {
        if (m_estop_active == true)
        {
            return; // cannot clear an e-stop fault while the e-stop is still active
        }
    }
    else if (m_active_fault == FaultCode::VISION_TIMEOUT)
    {
        m_watchdog.reset(); // reset watchdog to clear fault
    }

    // reset fault
    m_active_fault = FaultCode::NONE;
    m_state = CycleState::IDLE;
    m_fault_reset_requested = false;

    m_line.setConveyorRunning(true); // set conveyor running
}

uint16_t sentinel::Controller::getRegisters(RegisterIndex index)
{
    return registers[index];
}
