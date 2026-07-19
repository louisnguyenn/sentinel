#include "photoeye.hpp"

/// @brief state of the photoeye sensor (HIGH if object, LOW if not object present)
/// @param dt_s 
/// @param part_physically_present 
void sentinel::Photoeye::update(double dt_s, bool part_physically_present)
{
    if (part_physically_present != m_pending_state_)
    {
        // restart debounce timer
        m_pending_state_ = part_physically_present;
        m_time_in_pending_state_s_ = 0.0;
    }
    else
    {
        m_time_in_pending_state_s_ += dt_s;
        
        if (m_time_in_pending_state_s_ >= m_debounce_time_s_ && m_reported_state_ != m_pending_state_)
        {
            m_reported_state_ = m_pending_state_;
        }
    }
}

/// @brief return reported state
/// @return private boolean
bool sentinel::Photoeye::isBlocked() const
{
    return m_reported_state_;
}
