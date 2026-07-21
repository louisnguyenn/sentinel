#include "diverter_cylinder.hpp"

sentinel::DiverterCylinder::DiverterCylinder(double travel_time_s) : m_travel_time_s{travel_time_s}
{
}

/// true = command extend, false = command retract.
void sentinel::DiverterCylinder::command(bool extend)
{
}

void sentinel::DiverterCylinder::update(double dt_s)
{
}

sentinel::DiverterCylinder::Position sentinel::DiverterCylinder::position() const
{
}

/// 0.0 = fully retracted, 1.0 = fully extended — feed this into
/// ProximitySensor::update().
double sentinel::DiverterCylinder::positionFraction() const
{
}
