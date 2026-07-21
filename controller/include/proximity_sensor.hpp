#ifndef PROXIMITY_SENSOR_HPP
#define PROXIMITY_SENSOR_HPP

namespace sentinel {

/// Simulates a pair of proximity sensors (one at each end of travel)
/// reading the diverter cylinder's actual position.
class ProximitySensor {
public:
    ProximitySensor(double retracted_threshold = 0.05,
                     double extended_threshold = 0.95);

    /// position_fraction: 0.0 = fully retracted, 1.0 = fully extended.
    void update(double position_fraction);

    bool isRetracted() const;
    bool isExtended() const;
    /// True while the cylinder is between the two sensors (in transit).
    bool isInTransit()   const;

private:
    double m_retracted_threshold;
    double m_extended_threshold;
    bool m_retracted = true;
    bool m_extended = false;
};

} // namespace sentinel

#endif
