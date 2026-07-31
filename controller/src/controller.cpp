#include "controller.hpp"

// public methods
sentinel::Controller::Controller(ConveyorLine& line) : m_line{line}
{
}

void sentinel::Controller::tick(double dt_s)
{
}

void sentinel::Controller::setEstop(bool active)
{
}

void sentinel::Controller::requestFaultReset()
{
}

void sentinel::Controller::setMode(OperatingMode mode)
{
}

void sentinel::Controller::submitInspectionResult(bool defective)
{
}

void sentinel::Controller::feedVisionHeartbeat(uint16_t heartbeat_value)
{
}

sentinel::CycleState sentinel::Controller::state() const
{
}

sentinel::OperatingMode sentinel::Controller::mode() const
{
}

sentinel::FaultCode sentinel::Controller::activeFault() const
{
}

const sentinel::Stats& sentinel::Controller::stats() const
{
}

// private methods
void sentinel::Controller::inputScan()
{
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
            if (m_photoeye_snapshot == true)
            {
                m_state = CycleState::PART_DETECTED;
            }
            break;
        case CycleState::PART_DETECTED:
            m_has_inspection_result = false; // no result yet
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

                m_state = CycleState::DIVERT_ACCEPT;
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
                attemptFaultReset();    // call fault reset
            }
            break;
    }
}

void sentinel::Controller::outputScan()
{
}

void sentinel::Controller::housekeeping()
{
}

void sentinel::Controller::enterFault(FaultCode code)
{
}

void sentinel::Controller::attemptFaultReset()
{
}
