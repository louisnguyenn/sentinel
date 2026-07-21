#include "diverter_cylinder.hpp"

sentinel::DiverterCylinder::DiverterCylinder(double travel_time_s) : m_travel_time_s{travel_time_s}
{
}

void sentinel::DiverterCylinder::command(bool extend)
{
    m_commanded_extend = extend;
}

void sentinel::DiverterCylinder::update(double dt_s)
{
    int direction{};

    if (m_commanded_extend == true)
    {
        direction = 1; // extended
    }
    else
    {
        direction = -1; // retracted
    }

    double fraction_per_second = 1.0 / m_travel_time_s;

    m_position_fraction += direction * fraction_per_second * dt_s; // calculate position

    if (m_position_fraction <= 0.0)
    {
        m_position = Position::RETRACTED;
    }
    else if (m_position_fraction >= 1.0)
    {
        m_position = Position::EXTENDED;
    }
    else
    {
        m_commanded_extend ? m_position = Position::EXTENDING : m_position = Position::RETRACTING;
    }
}

sentinel::DiverterCylinder::Position sentinel::DiverterCylinder::position() const
{
    return m_position;
}

double sentinel::DiverterCylinder::positionFraction() const
{
}
