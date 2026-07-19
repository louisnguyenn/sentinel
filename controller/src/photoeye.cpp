#include "photoeye.hpp"

/// Advance the simulated sensor by dt_s, given the *true* physical
/// state (whether a part is physically breaking the beam right now).
void sentinel::Photoeye::update(double dt_s, bool part_physically_present)
{

}

/// Debounced, reportable state.
bool sentinel::Photoeye::isBlocked() const
{
    return m_reported_state_;
}
