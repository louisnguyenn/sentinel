#ifndef PHOTOEYE_HPP
#define PHOTOEYE_HPP

namespace sentinel
{

/// Simulates a through-beam photoeye sensor at the inspection zone.
/// Real photoeyes debounce: a state change must persist for
/// `debounce_time_s_` before it is reported as "real".
class Photoeye
{
  public:
    explicit Photoeye(double debounce_time_s = 0.01);

    /// Advance the simulated sensor by dt_s, given the *true* physical
    /// state (whether a part is physically breaking the beam right now).
    void update(double dt_s, bool part_physically_present);

    /// Debounced, reportable state.
    bool isBlocked() const;

  private:
    double debounce_time_s_;
    double time_in_pending_state_s_ = 0.0;
    bool pending_state_ = false;
    bool reported_state_ = false;
};

} // namespace sentinel

#endif // PHOTOEYE_HPP
