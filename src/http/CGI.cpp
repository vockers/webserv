#include "http/CGI.hpp"

#include <sys/stat.h>  // For stat
#include <sys/wait.h>
#include <unistd.h>

#include <algorithm>
#include <cstring>
#include <iostream>

#include "http/Response.hpp"

namespace webserv::http
{
using utils::ErrorLogger;

CGI::CGI(const Request& request, const std::string& uri, const std::string& interpreter)
    : _request(request)
{
    try {
        try_file(uri);
    } catch (Response::StatusCode code) {
        throw code;
    }

    int pipe_fd[2];
    if (pipe(pipe_fd) == -1) {
        throw Response::StatusCode::INTERNAL_SERVER_ERROR;
    }

    pid_t pid = fork();
    if (pid < 0) {
        throw Response::StatusCode::INTERNAL_SERVER_ERROR;
    }

    if (pid == 0) {
        close(pipe_fd[0]);
        dup2(pipe_fd[1], STDOUT_FILENO);
        close(pipe_fd[1]);

        char** env    = create_envp();
        char*  argv[] = {
            const_cast<char*>(interpreter.c_str()), const_cast<char*>(uri.c_str()), nullptr};
        if (execve(interpreter.c_str(), argv, env) == -1) {
            free_envp(env);
            exit(EXIT_FAILURE);
        }
    } else {
        close(pipe_fd[1]);

        int status;
        waitpid(pid, &status, 0);
        if (WIFEXITED(status) && WEXITSTATUS(status) != 0) {
            throw Response::StatusCode::INTERNAL_SERVER_ERROR;
        }

        char    buffer[BUFSIZE];
        ssize_t bytes_read;
        _output.clear();
        while ((bytes_read = read(pipe_fd[0], buffer, BUFSIZE)) > 0) {
            _output.append(buffer, bytes_read);
        }
        close(pipe_fd[0]);
    }
}

void CGI::try_file(const std::string& uri) const
{
    struct stat sb;
    if (stat(uri.c_str(), &sb) == -1) {
        throw Response::StatusCode::NOT_FOUND;
    }

    if (!(sb.st_mode & S_IXUSR)) {
        throw Response::StatusCode::FORBIDDEN;
    }
}

char** CGI::create_envp() const
{
    std::unordered_map<std::string, std::string> env_map;

    // Add standard CGI environment variables from the Request object
    env_map["REQUEST_METHOD"]  = (_request.get_method() == Request::Method::GET) ? "GET" : "POST";
    env_map["REQUEST_URI"]     = _request.get_uri();
    env_map["QUERY_STRING"]    = _request.get_query();
    env_map["SERVER_PROTOCOL"] = "HTTP/1.1";

    // Dynamically convert HTTP headers to CGI environment variables
    const Request::Headers& headers = _request.get_headers();
    for (const auto& [key, value] : headers) {
        std::string env_key;
        // Special cases for Content-Type and Content-Length (without HTTP_ prefix)
        if (key == "Content-Type") {
            env_key = "CONTENT_TYPE";
        } else if (key == "Content-Length") {
            env_key = "CONTENT_LENGTH";
        } else {
            // For other headers, dynamically apply the CGI transformation
            env_key = "HTTP_" + key;  // Prefix with HTTP_ (TODO: check if this is correct)
            std::transform(env_key.begin(),
                           env_key.end(),
                           env_key.begin(),
                           ::toupper);  // Convert to uppercase
            std::replace(
                env_key.begin(), env_key.end(), '-', '_');  // Replace hyphens with underscores
        }
        // Add the transformed header to the environment map
        env_map[env_key] = value;
    }
    return convert_map_to_envp(env_map);
}

char** CGI::convert_map_to_envp(const std::unordered_map<std::string, std::string>& env_map) const
{
    char** envp = new char*[env_map.size() + 1];

    size_t i = 0;
    for (const auto& [key, value] : env_map) {
        std::string env = key + "=" + value;
        envp[i]         = new char[env.size() + 1];
        std::strcpy(envp[i], env.c_str());
        ++i;
    }
    envp[i] = nullptr;
    return envp;
}

void CGI::free_envp(char** envp) const
{
    for (size_t i = 0; envp[i] != nullptr; ++i) {
        delete[] envp[i];
    }
    delete[] envp;
}

bool CGI::is_cgi_request(const std::string& uri, std::string& interpreter)
{
    std::unordered_map<std::string, std::string> cgi_interpreters = {
        {".py", "/bin/python3"},
        // {".php", "/usr/bin/php-cgi"},
        // {".pl", "/usr/bin/perl"},
        // {".rb", "/usr/bin/ruby"}
    };

    for (const auto& [extension, path] : cgi_interpreters) {
        size_t ext_size = extension.size();
        if (uri.size() >= ext_size &&
            uri.compare(uri.size() - ext_size, ext_size, extension) == 0) {
            interpreter = path;
            return true;
        }
    }
    return false;
}

std::string CGI::get_output() const
{
    return _output;
}
}  // namespace webserv::http
