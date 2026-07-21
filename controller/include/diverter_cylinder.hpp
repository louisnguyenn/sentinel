#ifndef DIVERTER_CYLINDER_HPP
#define DIVERTER_CYLINDER_HPP

namespace sentinel {

/// Simulates a pneumatic diverter cylinder with finite travel time.
class DiverterCylinder {
public:
    enum class Position { RETRACTED, EXTENDING, EXTENDED, RETRACTING };

    explicit DiverterCylinder(double travel_time_s = 0.15);

    /// true = command extend, false = command retract.
    void command(bool extend);

    void update(double dt_s);

    Position position() const;

    /// 0.0 = fully retracted, 1.0 = fully extended — feed this into
    /// ProximitySensor::update().
    double positionFraction() const;

private:
    double m_travel_time_s;
    bool m_commanded_extend = false;
    double m_position_fraction = 0.0; // 0..1
    Position m_position = Position::RETRACTED;
};

} // namespace sentinel

#endif
