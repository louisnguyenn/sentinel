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
    return m_retracted;
}

bool sentinel::ProximitySensor::isExtended() const
{
    return m_extended;
}

bool sentinel::ProximitySensor::isInTransit() const
{
    return !m_retracted && !m_extended; // neither extended or retracted
}
