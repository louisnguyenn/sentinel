#include "conveyor_motor.hpp"

#include <algorithm>

// constructor
sentinel::ConveyorMotor::ConveyorMotor(double rated_speed_mm_s, double accel_mm_s2) : m_rated_speed_mm_s{rated_speed_mm_s}, m_accel_mm_s2{accel_mm_s2}
{
}

void sentinel::ConveyorMotor::setRunning(bool run)
{
    m_running = run;
}

void sentinel::ConveyorMotor::update(double dt_s)
{
    double target_speed{};
    double max_delta_this_tick{};

    m_running ? target_speed = m_rated_speed_mm_s : target_speed = 0.0;

    max_delta_this_tick = m_accel_mm_s2 * dt_s; // calculate displacement

    // rate limiter
    // ramp up speed by change in tick instead of jumping straight to the target speed
    if (m_current_speed_mm_s < target_speed)
    {
        m_current_speed_mm_s = std::min(m_current_speed_mm_s + max_delta_this_tick, target_speed);
    }
    else if (m_current_speed_mm_s > target_speed)
    {
        m_current_speed_mm_s = std::max(m_current_speed_mm_s - max_delta_this_tick, target_speed);
    }
    
}

double sentinel::ConveyorMotor::currentSpeedMmS() const
{
}

bool sentinel::ConveyorMotor::isRunning() const
{
}
