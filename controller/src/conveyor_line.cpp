#include "conveyor_line.hpp"

// public
// constuctor
sentinel::ConveyorLine::ConveyorLine() = default;

void sentinel::ConveyorLine::update(double dt_s)
{
    m_motor.update(dt_s);

    for (auto &part : m_parts)
    {
        part.position_mm += m_motor.currentSpeedMmS() * dt_s;
    }

    for (size_t i{0}; i < m_parts.size();)
    {
        // check if parts are past the line
        if (m_parts[i].position_mm > kLineLengthMm)
        {
            // remove the part (they've left the line)
            m_parts.erase(m_parts.begin() + i);
        }
        else
        {
            ++i;
        }
    }

    // update components of the line
    m_photoeye.update(dt_s, anyPartInInspectionZone()); // check if part is physically in the inspection zone
    m_encoder.update(dt_s, m_motor.currentSpeedMmS());  // count how far the belt has moved using pulses
    m_diverter.update(dt_s);    //  time speed accurately to divert the defected part
    m_proximity.update(m_diverter.positionFraction());  // check if the arm has fully extended or not (stuck on extending)
}

void sentinel::ConveyorLine::spawnPart()
{
    m_parts.push_back(Part{m_next_part_id++, 0.0});
}

bool sentinel::ConveyorLine::photoeyeBlocked() const
{
    return m_photoeye.isBlocked();
}

uint32_t sentinel::ConveyorLine::encoderCount() const
{
    return m_encoder.pulseCount();
}

bool sentinel::ConveyorLine::diverterExtended() const
{
    return m_diverter.position() == sentinel::DiverterCylinder::Position::EXTENDED;
}

bool sentinel::ConveyorLine::diverterRetracted() const
{
    return m_diverter.position() == sentinel::DiverterCylinder::Position::RETRACTED;
}

void sentinel::ConveyorLine::commandDiverter(bool extend)
{
    m_diverter.command(extend);
}

void sentinel::ConveyorLine::setConveyorRunning(bool run)
{
    m_motor.setRunning(run);
}

double sentinel::ConveyorLine::motorSpeedMmS()
{
    return m_motor.currentSpeedMmS();
}

// private
bool sentinel::ConveyorLine::anyPartInInspectionZone() const
{
    double window{10}; // 10mm window around inspection zone

    for (auto &part : m_parts)
    {
        // position is greater than lower bound and less than upper bound
        if (part.position_mm < kInspectionZoneMm + window && part.position_mm > kInspectionZoneMm - window)
        {
            return true;
        }
    }

    return false;
}
