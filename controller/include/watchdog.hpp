#ifndef WATCHDOG_HPP
#define WATCHDOG_HPP

#include <chrono>
#include <cstdint>

namespace sentinel
{

/// Detects a stalled or unresponsive vision process by watching a
/// heartbeat value that vision is expected to change every cycle.
class Watchdog
{
  public:
    explicit Watchdog(std::chrono::milliseconds timeout = std::chrono::milliseconds(800));

    /// Call this whenever a new heartbeat value arrives from vision.
    void feed(uint16_t heartbeat_value);

    /// True if the heartbeat hasn't changed within the timeout window.
    bool timedOut() const;

    /// Clears the timeout condition and restarts the timer from now.
    void reset();

  private:
    std::chrono::milliseconds m_timeout;
    uint16_t m_last_seen = 0;
    std::chrono::steady_clock::time_point m_last_change;
};

} // namespace sentinel

#endif
