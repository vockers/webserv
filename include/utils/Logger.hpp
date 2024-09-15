#pragma once

#include <string>
#include <sstream>

namespace webserv::utils
{
#define LOG(level, message) Logger(level, message)
#define LOG_DEBUG(message) LOG(Logger::Level::DEBUG, message
#define LOG_INFO(message) LOG(Logger::Level::INFO, message)
#define LOG_WARNING(message) LOG(Logger::Level::WARNING, message)
#define LOG_ERROR(message) LOG(Logger::Level::ERROR, message)
#define LOG_CRITICAL(message) LOG(Logger::Level::CRITICAL, message)

class Logger
{
public:
    enum class Level
    {
        DEBUG,
        INFO,
        WARNING,
        ERROR,
        CRITICAL
    };
    Logger(Level level, const std::string& message);
    ~Logger();

    static const char* level_to_string(Level level);

private:
    std::ostringstream _buffer;

    std::string get_timestamp();
};
}
