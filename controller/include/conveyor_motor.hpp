#ifndef CONVEYOR_MOTOR_HPP
#define CONVEYOR_MOTOR_HPP

namespace sentinel {

/// Simulates the conveyor drive motor with simple linear ramping.
class ConveyorMotor {
public:
    ConveyorMotor(double rated_speed_mm_s = 200.0,
                   double accel_mm_s2 = 500.0);

    void setRunning(bool run);
    void update(double dt_s);

    double currentSpeedMmS() const;
    bool isRunning() const;

private:
    double m_rated_speed_mm_s;
    double m_accel_mm_s2;
    bool m_running = false;
    double m_current_speed_mm_s = 0.0;
};

} // namespace sentinel

#endif
