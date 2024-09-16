#pragma once

#include <string>

namespace webserv::utils
{
class Logger
{
public:
    Logger();

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

    void log(Level level, const std::string& message);

private:
    Level _level;
};
}  // namespace webserv::utils
