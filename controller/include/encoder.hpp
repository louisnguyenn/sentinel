#ifndef LINEGUARD_ENCODER_HPP
#define LINEGUARD_ENCODER_HPP

#include <cstdint>

namespace sentinel {

/// Simulates an incremental encoder on the conveyor drive shaft.
class Encoder {
public:
    explicit Encoder(double pulses_per_mm = 10.0);

    /// Advance the encoder given the conveyor's current linear speed.
    void update(double dt_s, double conveyor_speed_mm_s);

    uint32_t pulseCount() const;

    /// Distance implied by the pulse count, in mm.
    double distanceMm() const;

    void reset();

private:
    double pulses_per_mm_;
    double fractional_pulse_accumulator_ = 0.0;
    uint32_t pulse_count_ = 0;
};

} // namespace sentinel

#endif
