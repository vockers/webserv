#include "http/CGI.hpp"

#include <sys/stat.h>  // For stat
#include <sys/wait.h>
#include <unistd.h>

#include <algorithm>
#include <cstring>

#include "http/Response.hpp"
#include "utils/std_utils.hpp"

#ifndef BUFFER_SIZE
#define BUFFER_SIZE 4096
#endif

namespace webserv::http
{
CGI::CGI(const Request& request, const std::string& uri, const std::string& interpreter)
    : _request(request), _state(State::IDLE), _bytes_written(0)
{
    this->try_file(uri);

    if (pipe(_stdout_pipe) == -1 || pipe(_stdin_pipe) == -1) {
        throw Response::StatusCode::INTERNAL_SERVER_ERROR;
    }

    _pid = fork();
    if (_pid < 0) {
        throw Response::StatusCode::INTERNAL_SERVER_ERROR;
    }

    if (_pid == 0) {
        close(_stdout_pipe[0]);
        dup2(_stdout_pipe[1], STDOUT_FILENO);
        close(_stdout_pipe[1]);

        close(_stdin_pipe[1]);
        dup2(_stdin_pipe[0], STDIN_FILENO);
        close(_stdin_pipe[0]);

        char** env    = create_envp();
        char*  argv[] = {
            const_cast<char*>(interpreter.c_str()), const_cast<char*>(uri.c_str()), nullptr};

        if (execve(interpreter.c_str(), argv, env) == -1) {
            utils::free_string_array(env);
            exit(EXIT_FAILURE);
        }
    } else {
        close(_stdout_pipe[1]);
        close(_stdin_pipe[0]);
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
    env_map["REQUEST_METHOD"] = (_request.get_method() == Request::Method::GET) ? "GET" : "POST";
    if (_request.get_method() == Request::Method::POST) {
        env_map["CONTENT_LENGTH"] = std::to_string(_request.body().size());
    }
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

CGI::State CGI::state() const
{
    return _state;
}

Promise<ssize_t> CGI::read(std::string& buffer)
{
    return Promise<ssize_t>(
        [this, &buffer]() -> std::optional<ssize_t> {
            buffer.resize(BUFFER_SIZE);
            ssize_t bytes_read = ::read(_stdout_pipe[0], buffer.data(), BUFFER_SIZE);
            buffer.resize(bytes_read);
            if (bytes_read == -1) {
                return std::nullopt;
            }
            return bytes_read;
        },
        _stdout_pipe[0],
        async::Event::READABLE);
}

Promise<ssize_t> CGI::write(const std::string& buffer)
{
    return Promise<ssize_t>(
        [this, &buffer]() -> std::optional<ssize_t> {
            ssize_t bytes_written = ::write(_stdin_pipe[1], buffer.data(), buffer.size());
            if (bytes_written == -1) {
                return std::nullopt;
            }
            return bytes_written;
        },
        _stdin_pipe[1],
        async::Event::WRITABLE);
}

Promise<std::string> CGI::get_output()
{
    _state = State::WRITE;

    return Promise<std::string>([this]() -> std::optional<std::string> {
        if (_state == State::WRITE) {
            this->write(_request.body()).then([this](ssize_t bytes_written) {
                _bytes_written += bytes_written;
                if (_bytes_written >= _request.body().size()) {
                    close(_stdin_pipe[1]);
                    int status;
                    waitpid(_pid, &status, 0);
                    if (WIFEXITED(status) && WEXITSTATUS(status) != 0) {
                        throw Response::StatusCode::INTERNAL_SERVER_ERROR;
                    }
                    _state = State::READ;
                }
            });
        }
        if (_state == State::READ) {
            std::string buffer;
            this->read(buffer).then([this, &buffer](ssize_t bytes_read) {
                if (bytes_read == 0) {
                    _state = State::DONE;
                    close(_stdout_pipe[0]);
                } else {
                    _output += buffer;
                }
            });
        }
        if (_state == State::DONE) {
            return _output;
        }
        return std::nullopt;
    });
}
}  // namespace webserv::http
