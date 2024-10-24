#pragma once

#include "http/Request.hpp"
#include "utils/Logger.hpp"

#define BUFSIZE 4096

namespace webserv::http
{
using utils::ErrorLogger;  // inside or outside of the class?

class Cgi
{
public:
    Cgi(const Request& request, const std::string& uri, const std::string& interpreter);

    std::string get_output() const;
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
