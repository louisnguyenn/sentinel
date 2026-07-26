#include "watchdog.hpp"

// constructor
sentinel::Watchdog::Watchdog(std::chrono::milliseconds timeout) : m_timeout{timeout}
{
}

// if heartbeat value is the same as last seen heartbeat value, do nothing; still waiting
void sentinel::Watchdog::feed(uint16_t heartbeat_value)
{
    if (heartbeat_value != m_last_seen)
    {
        m_last_seen = heartbeat_value;
        m_last_change = std::chrono::steady_clock::now();
    }
}

// returns true if not changed
bool sentinel::Watchdog::timedOut() const
{
    return (std::chrono::steady_clock::now() - m_last_change) > m_timeout;
}

// reset time
void sentinel::Watchdog::reset()
{
    m_last_change = std::chrono::steady_clock::now();
}
