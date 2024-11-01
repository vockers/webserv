#pragma once

#include "async/Promise.hpp"
#include "http/Request.hpp"
#include "utils/Logger.hpp"

namespace webserv::http
{
using async::Promise;
using utils::ErrorLogger;

class CGI
{
public:
    enum class State
    {
        IDLE,
        READ,
        WRITE,
        DONE
    };

    /// @brief Creates a new process to execute the CGI script
    ///
    /// @param request the request object
    /// @param uri path of the request
    /// @param interpreter path to the interpreter
    CGI(const Request& request, const std::string& uri, const std::string& interpreter);

    Promise<std::string> get_output();

    /// @brief Checks if the request is a CGI request and sets the interpreter
    ///
    /// @param uri path of the request
    /// @param interpreter path to the interpreter
    /// @return true if the request is a CGI request, false otherwise
    static bool is_cgi_request(const std::string& uri, std::string& interpreter);

    State state() const;

    Promise<ssize_t> read(std::string& buffer);
    Promise<ssize_t> write(const std::string& buffer);

private:
    State          _state;
    size_t         _bytes_written;
    std::string    _buffer;
    std::string    _output;
    const Request& _request;

    pid_t _pid;
    int   _stdin_pipe[2];
    int   _stdout_pipe[2];

    char** create_envp() const;
    char** convert_map_to_envp(const std::unordered_map<std::string, std::string>& env_map) const;
    void   try_file(const std::string& uri) const;
};
}  // namespace webserv::http
