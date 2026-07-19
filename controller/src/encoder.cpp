#include "encoder.hpp"

#include <cmath>

/// @brief update the encoder with the speed of the conveyor belt through pulses
/// @param dt_s
/// @param conveyor_speed_mm_s
void sentinel::Encoder::update(double dt_s, double conveyor_speed_mm_s)
{
    double distance_this_tick_mm = conveyor_speed_mm_s * dt_s;          // calculate the distance from speed * time
    double pulses_this_tick = distance_this_tick_mm * m_pulses_per_mm_; // calculate pulses each tick

    // round down when counting pulses at low speeds - accumulate fractional part for accuracy
    m_fractional_pulse_accumulator_ += pulses_this_tick;
    double whole_pulses = std::floor(m_fractional_pulse_accumulator_); // round down
    m_pulse_count += static_cast<uint32_t> whole_pulses;
    m_fractional_pulse_accumulator_ -= whole_pulses;
}

uint32_t sentinel::Encoder::pulseCount() const
{
    return m_pulse_count;
}

/// Distance implied by the pulse count, in mm.
double sentinel::Encoder::distanceMm() const
{
    return static_cast<double>(m_pulse_count) / m_pulses_per_mm_;
}

/// @brief zero the encoder
void sentinel::Encoder::reset()
{
    // reset acculumations
    m_pulse_count = 0;
    m_fractional_pulse_accumulator_ = 0;.0
}
