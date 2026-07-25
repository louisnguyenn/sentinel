#include "conveyor_line.hpp"

void sentinel::ConveyorLine::update(double dt_s)
{
    m_motor.update(dt_s);
    int i{};

    for (auto &part : m_parts)
    {
        part.position_mm += m_motor.currentSpeedMmS() * dt_s;
    }

    for (size_t i{0}; i < m_parts.size();)
    {
        if (m_parts[i].position_mm > kLineLengthMm)
        {
            m_parts.erase(m_parts.begin() + i);
        }
        else
        {
            ++i;
        }
    }

    m_photoeye.update(dt_s, anyPartInInspectionZone());
    m_encoder.update(dt_s, m_motor.currentSpeedMmS());
    m_diverter.update(dt_s);
    m_proximity.update(m_diverter.positionFraction());
}

void sentinel::ConveyorLine::spawnPart()
{
    m_parts.push_back(Part {id: m_next_part_id++, position_mm: 0.0});
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
    m_diverter.command(extend);
}

void sentinel::ConveyorLine::setConveyorRunning(bool run)
{
    m_motor.setRunning(run);
}
