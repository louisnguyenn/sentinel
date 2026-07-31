#ifndef LOGGER_HPP
#define LOGGER_HPP

#include <string>

namespace sentinel
{

enum class LogLevel { DEBUG, INFO, WARN, ERROR };

/// A minimal, single-instance logger writing timestamped, severity-tagged
/// messages to console and (optionally) a file.
class Logger
{
  public:
    static Logger &instance();

    void log(LogLevel level, const std::string &message);
    void setMinLevel(LogLevel level);

  private:
    Logger() = default;
    LogLevel m_min_level = LogLevel::INFO;
};

} // namespace sentinel

#endif