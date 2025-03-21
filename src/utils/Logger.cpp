#include "utils/Logger.hpp"

#include <chrono>
#include <iostream>
#include <sstream>

#include "utils/Color.hpp"

namespace webserv::utils
{
Logger::Logger() : _stream(std::cerr) {}

std::string Logger::get_timestamp()
{
    auto        now  = std::chrono::system_clock::now();
    std::time_t time = std::chrono::system_clock::to_time_t(now);
    char        buffer[20];

    std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", std::localtime(&time));
    return std::string(buffer);
}

ErrorLogger::ErrorLogger(Level level) : _level(level) {}

void ErrorLogger::log(ErrorLogger::Level level, const std::string& message)
{
    if (level < _level) {
        return;
    }

    std::stringstream buffer;
    buffer << "[" << get_timestamp() << "]";
    switch (level) {
    case Level::DEBUG:
        buffer << Color::CYAN << "[DEBUG]";
        break;
    case Level::INFO:
        buffer << Color::GREEN << "[INFO]";
        break;
    case Level::WARNING:
        buffer << Color::YELLOW << "[WARNING]";
        break;
    case Level::ERROR:
        buffer << Color::RED << "[ERROR]";
        break;
    case Level::CRITICAL:
        buffer << Color::MAGENTA << "[CRITICAL]";
        break;
    }

    buffer << Color::RESET << ": " << message << std::endl;
    _stream << buffer.str();
}

void ErrorLogger::log(const std::string& message)
{
    log(Level::DEBUG, message);
}

void ErrorLogger::set_level(ErrorLogger::Level level)
{
    _level = level;
}

void ErrorLogger::set_level(const std::string& level)
{
    if (level == "debug") {
        _level = Level::DEBUG;
    } else if (level == "info") {
        _level = Level::INFO;
    } else if (level == "warn") {
        _level = Level::WARNING;
    } else if (level == "error") {
        _level = Level::ERROR;
    } else if (level == "crit") {
        _level = Level::CRITICAL;
    }
}
}  // namespace webserv::utils
