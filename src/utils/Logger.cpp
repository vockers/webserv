#include "utils/Logger.hpp"

#include <iostream>
#include <chrono>

namespace webserv::utils
{
Logger::Logger(Level level, const std::string& message)
{
    _buffer << "[" << get_timestamp() << "] [" << level_to_string(level) << "] " << message << std::endl;
}

Logger::~Logger()
{
    std::cout << _buffer.str();
}
const char* Logger::level_to_string(Level level)
{
    switch (level) {
    case Level::DEBUG:
        return "DEBUG";
    case Level::INFO:
        return "INFO";
    case Level::WARNING:
        return "WARNING";
    case Level::ERROR:
        return "ERROR";
    case Level::CRITICAL:
        return "CRITICAL";
    default:
        return "UNKNOWN";
    }
}

std::string Logger::get_timestamp()
{
    auto now = std::chrono::system_clock::now();
    std::time_t time = std::chrono::system_clock::to_time_t(now);
    char buffer[20];

    std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", std::localtime(&time));
    return std::string(buffer);
}
}
