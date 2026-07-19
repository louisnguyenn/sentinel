#include "encoder.hpp"

/// @brief update the encoder with the speed of the conveyor belt through pulses
/// @param dt_s
/// @param conveyor_speed_mm_s
void sentinel::Encoder::update(double dt_s, double conveyor_speed_mm_s)
{
    distance_this_tick_mm = conveyor_speed_mm_s * dt_s;       // calculate the distance from speed * time
    pulses_this_tick = distance_this_tick * m_pulses_per_mm_; // calculate pulses each tick

    // round down when counting pulses at low speeds - accumulate fractional part for accuracy
    m_fractional_pulse_accumulator_ += pulses_this_tick;
    whole_pulses = floor(m_fractional_pulse_accumulator_); // round down
    m_pulse_count += whole_pulses;
    m_fractional_pulse_accumulator_ -= whole_pulses;
}

uint32_t sentinel::Encoder::pulseCount() const
{
}

/// Distance implied by the pulse count, in mm.
double sentinel::Encoder::distanceMm() const
{
}

void sentinel::Encoder::reset()
{
}
