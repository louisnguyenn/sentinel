#include "proximity_sensor.hpp"

// constuctor
sentinel::ProximitySensor::ProximitySensor(double retracted_threshold, double extended_threshold)
    : m_retracted_threshold(retracted_threshold), m_extended_threshold(extended_threshold)
{
}

void sentinel::ProximitySensor::update(double position_fraction)
{
    m_retracted = position_fraction <= m_retracted_threshold;
    m_extended = position_fraction >= m_extended_threshold;
}

bool sentinel::ProximitySensor::isRetracted() const
{
}

bool sentinel::ProximitySensor::isExtended() const
{
}

/// True while the cylinder is between the two sensors (in transit).
bool sentinel::ProximitySensor::isInTransit() const
{
}
