#include "conveyor_motor.hpp"

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
}

double sentinel::ConveyorMotor::currentSpeedMmS() const
{
}

bool sentinel::ConveyorMotor::isRunning() const
{
}
