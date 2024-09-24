#pragma once

#include <string>
#include <unordered_map>

namespace webserv::http
{
class Request
{
public:
    using Headers = std::unordered_map<std::string, std::string>;

    enum class Method
    {
        GET,
        POST,
        DELETE,
    };

    Request(const std::string& input);

private:
    using MethodMap = std::unordered_map<std::string, Method>;

    Method  _method;
    Headers _headers;

    void parse_line(const std::string& input);
    void parse_headers(const std::string& input);

    static const MethodMap METHOD_MAP;
};
}
