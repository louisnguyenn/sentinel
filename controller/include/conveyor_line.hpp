#ifndef CONVEYOR_LINE_HPP
#define CONVEYOR_LINE_HPP

#include <vector>
#include "photoeye.hpp"
#include "encoder.hpp"
#include "proximity_sensor.hpp"
#include "conveyor_motor.hpp"
#include "diverter_cylinder.hpp"

namespace sentinel {

/// Owns the simulated field devices and a simple list of parts moving
/// down the conveyor. This is the "plant" the controller reads from
/// and writes to, standing in for real I/O.
class ConveyorLine {
public:
    ConveyorLine();

    /// Advance the whole simulated plant by one scan-cycle tick.
    void update(double dt_s);

    /// Introduce a new part at the start of the conveyor.
    void spawnPart();

    // --- Sensor reads (controller calls these during inputScan) ---
    bool photoeyeBlocked() const;
    uint32_t encoderCount() const;
    bool diverterExtended() const;
    bool diverterRetracted() const;

    // --- Actuator commands (controller calls these during outputScan) ---
    void commandDiverter(bool extend);
    void setConveyorRunning(bool run);

private:
    struct Part {
        int id;
        double position_mm;
        bool diverted = false;
    };

    static constexpr double kInspectionZoneMm = 500.0;
    static constexpr double kLineLengthMm = 1000.0;

    Photoeye m_photoeye;
    Encoder m_encoder;
    ProximitySensor m_proximity;
    ConveyorMotor m_motor;
    DiverterCylinder m_diverter;

    std::vector<Part> m_parts;
    int m_next_part_id = 1;

    /// True if any part currently overlaps the inspection zone.
    bool anyPartInInspectionZone() const;
};

} // namespace sentinel

#endif
