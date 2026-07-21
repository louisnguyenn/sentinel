#include "conveyor_line.hpp"

void sentinel::ConveyorLine::update(double dt_s)
{
}

void sentinel::ConveyorLine::spawnPart()
{
}

bool sentinel::ConveyorLine::photoeyeBlocked() const
{
}

uint32_t sentinel::ConveyorLine::encoderCount() const
{
}

bool sentinel::ConveyorLine::diverterExtended() const
{
}

bool sentinel::ConveyorLine::diverterRetracted() const
{
}

void sentinel::ConveyorLine::commandDiverter(bool extend)
{
}

void sentinel::ConveyorLine::setConveyorRunning(bool run)
{
    m_motor.setRunning(run);
}
