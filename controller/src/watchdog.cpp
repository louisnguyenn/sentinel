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

bool sentinel::Watchdog::timedOut() const
{
}

void sentinel::Watchdog::reset()
{
}
