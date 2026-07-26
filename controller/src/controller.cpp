#include "controller.hpp"

// public methods
sentinel::Controller::Controller(ConveyorLine &line) : m_line{line}
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

const sentinel::Stats &sentinel::Controller::stats() const
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
