#pragma once

#include "http/Request.hpp"
#include "utils/Logger.hpp"

#define BUFSIZE 4096

namespace webserv::http
{
using utils::ErrorLogger;

class CGI
{
public:
    /// @brief Creates a new process to execute the CGI script
	///
    /// @param request the request object
    /// @param uri path of the request
    /// @param interpreter path to the interpreter
    CGI(const Request& request, const std::string& uri, const std::string& interpreter);

    std::string get_output() const;

    /// @brief Checks if the request is a CGI request and sets the interpreter
    ///
	/// @param uri path of the request
    /// @param interpreter path to the interpreter
    /// @return true if the request is a CGI request, false otherwise
    static bool is_cgi_request(const std::string& uri, std::string& interpreter);

private:
    std::string    _output;
    const Request& _request;

    char** create_envp() const;
    char** convert_map_to_envp(const std::unordered_map<std::string, std::string>& env_map) const;
    void   free_envp(char** envp) const;
    void   try_file(const std::string& uri) const;
};
}  // namespace webserv::http
