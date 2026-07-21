#include "proximity_sensor.hpp"

// constuctoer
sentinel::ProximitySensor::ProximitySensor(double retracted_threshold, double extended_threshold)
    : m_retracted_threshold(retracted_threshold), m_extended_threshold(extended_threshold)
{
}

/// position_fraction: 0.0 = fully retracted, 1.0 = fully extended.
void update(double position_fraction)
{
}

bool isRetracted() const
{
}

bool isExtended() const
{
}

/// True while the cylinder is between the two sensors (in transit).
bool isInTransit() const
{
}