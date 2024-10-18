#pragma once

#include <string>

namespace webserv::utils
{
class Logger
{
public:
    Logger();

    /// @brief Returns the current timestamp as a string
    ///
    /// format: [YYYY-MM-DD HH:MM:SS]
    ///
    /// @return The current timestamp
    static std::string get_timestamp();

protected:
    std::ostream& _stream;
};

class ErrorLogger : public Logger
{
public:
    enum Level
    {
        DEBUG,
        INFO,
        WARNING,
        ERROR,
        CRITICAL
    };

    ErrorLogger(Level level);

    /// @brief Log a message with a specific level
    //
    // format: [TIMESTAMP][LEVEL]: `message`
    //
    // @param level The level of the message
    // @param message The message to log
    void log(Level level, const std::string& message);

    /// @brief Log a message with a level of DEBUG
    //
    // format: [TIMESTAMP][DEBUG]: `message`
    //
    // @param message The message to log
    void log(const std::string& message);

    /// @brief Set the level of the logger
    ///
    /// The logger will only log messages with a
    /// level greater or equal to the level set
    ///
    /// @param level The level of the logger
    void set_level(Level level);

    /// @brief Set the level of the logger
    ///
    /// The logger will only log messages with a
    /// level greater or equal to the level set
    ///
    /// @param level The level of the logger
    void set_level(const std::string& level);

private:
    Level _level;
};
}  // namespace webserv::utils
