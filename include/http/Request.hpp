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

    Method             get_method() const;
    const std::string& get_uri() const;
    const Headers&     get_headers() const;

private:
    using MethodMap = std::unordered_map<std::string, Method>;

    Method      _method;
    std::string _uri;
    Headers     _headers;

    void parse_line(const std::string& line);
    void parse_headers(const std::string& headers);

    static const MethodMap METHOD_MAP;
};
}
