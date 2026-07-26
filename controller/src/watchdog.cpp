#include "watchdog.hpp"

sentinel::Watchdog::Watchdog(std::chrono::milliseconds timeout) : m_timeout{timeout}
{
}

void sentinel::Watchdog::feed(uint16_t heartbeat_value)
{

}

bool sentinel::Watchdog::timedOut() const
{

}

void sentinel::Watchdog::reset()
{

}
