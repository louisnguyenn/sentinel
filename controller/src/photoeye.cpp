#include "photoeye.hpp"

// constructor
sentinel::Photoeye::Photoeye(double debounce_time_s = 0.01) : m_debounce_time_s{debounce_time_s}
{
}

/// @brief state of the photoeye sensor (HIGH if object, LOW if not object present)
/// @param dt_s
/// @param part_physically_present
void sentinel::Photoeye::update(double dt_s, bool part_physically_present)
{
    if (part_physically_present != m_pending_state)
    {
        // restart debounce timer
        m_pending_state = part_physically_present;
        m_time_in_pending_state_s = 0.0;
    }
    else
    {
        m_time_in_pending_state_s += dt_s;

        if (m_time_in_pending_state_s >= m_debounce_time_s && m_reported_state != m_pending_state)
        {
            m_reported_state = m_pending_state;
        }
    }
}

/// @brief return reported state
/// @return private boolean
bool sentinel::Photoeye::isBlocked() const
{
    return m_reported_state;
}
